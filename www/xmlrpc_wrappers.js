/**
 * JS-XMLRPC "wrapper" functions
 * Generate stubs to transparently access xmlrpc methods as js functions
 *
 * @version $Id: xmlrpc_wrappers.js,v 1.8 2008/03/07 16:57:54 ggiunta Exp $
 * @author Gaetano Giunta
 * @copyright (c) 2006-2009 G. Giunta
 * @license code licensed under the BSD License: http://phpxmlrpc.sourceforge.net/jsxmlrpc/license.txt
 */

	// requires: xmlrpc_lib.js

/**
* Given a string defining a phpxmlrpc type return corresponding js type.
* @param string xmlrpctype
* @return string
* @private
*/
function xmlrpc_2_js_type(xmlrpctype)
{
	switch(xmlrpctype.toLowerCase())
	{
		case 'base64':
		case 'string':
			return 'string';
		case 'datetime.iso8601':
			return 'Date';
		case 'int':
		case 'i4':
			return 'integer';
		case 'struct':
			return 'object';
		case 'array':
			return 'array';
		case 'double':
			return 'number';
		case 'undefined':
			return 'mixed';
		case 'boolean':
		case 'null':
		default:
			// unknown: might be any xmlrpc type
			return xmlrpctype.toLowerCase();
	}
}

/**
* Given an xmlrpc client and a method name, register a js wrapper function
* that will call it and return results using native js types for both
* params and results. The generated js function will return an xmlrpcresp
* oject for failed xmlrpc calls
*
* Known limitations:
* - server must support system.methodsignature for the wanted xmlrpc method
* - for methods that expose many signatures, only one can be picked (we
*   could in priciple check if signatures differ only by number of params
*   and not by type, but it would be more complication than we can spare time)
* - nested xmlrpc params: the caller of the generated js function has to
*   encode on its own the params passed to the js function if these are structs
*   or arrays whose (sub)members include values of type base64
*
* Notes: the connection properties of the given client will be copied
* and reused for the connection used during the call to the generated
* js function.
* Calling the generated js function 'might' be slow: a new xmlrpc client
* is created on every invocation and an xmlrpc-connection opened+closed.
* An extra 'debug' param is appended to param list of xmlrpc method, useful
* for debugging purposes.
*
* @param xmlrpc_client client     an xmlrpc client set up correctly to communicate with target server
* @param string        methodname the xmlrpc method to be mapped to a js function
* @param array         extra_options map of options that specify conversion details. valid ptions include
*        integer       signum     the index of the method signature to use in mapping (if method exposes many sigs)
*        integer       timeout    timeout (in secs) to be used when executing function/calling remote method
*        string        protocol   'http' (default), 'http11' or 'https'
*        string        new_function_name the name of js function to create. If unsepcified, lib will pick an appropriate name
*        string        return_source if true return js code w. function definition instead of created function
*        bool          encode_js_objs let js objects be sent to server using the 'improved' xmlrpc notation, so server can deserialize them as js objects
*        bool          decode_js_objs --- WARNING !!! possible security hazard. only use it with trusted servers ---
*        mixed         return_on_fault a js value to be returned when the xmlrpc call fails/returns a fault response (by default the xmlrpcresp object is returned in this case). If a string is used, '%faultCode%' and '%faultString%' tokens will be substituted with actual error values
*        bool          debug      set it to 1 or 2 to see debug results of querying server for method synopsis
* @return string                  the generated js function (or false) - OR AN ARRAY...
* @public
*/
function wrap_xmlrpc_method(client, methodname, extra_options)
{
	if (extra_options === undefined) extra_options = {};
	var signum = extra_options['signum'] != undefined ? parseInt(extra_options['signum']) : 0;
	var timeout = extra_options['timeout'] != undefined ? parseInt(extra_options['timeout']) : 0;
	var protocol = extra_options['protocol'] != undefined ? extra_options['protocol'] : '';
	var newfuncname = extra_options['new_function_name'] != undefined ? extra_options['new_function_name'] : '';

	//$encode_php_objects = in_array('encode_php_objects', $extra_options);
	//$verbatim_client_copy = in_array('simple_client_copy', $extra_options) ? 1 :
	//	in_array('build_class_code', $extra_options) ? 2 : 0;

	var encode_js_objects = extra_options['encode_js_objs'] != undefined ? Boolean(extra_options['encode_js_objs']) : false;
	var decode_js_objects = extra_options['decode_js_objs'] != undefined ? Boolean(extra_options['decode_js_objs']) : false;
	var simple_client_copy = extra_options['simple_client_copy'] != undefined ? parseInt(extra_options['simple_client_copy']) : 0;
	var buildit = extra_options['return_source'] != undefined ? !(extra_options['return_source']) : true;
	var prefix = extra_options['prefix'] != undefined ? extra_options['prefix'] : 'xmlrpc';
	if (extra_options['return_on_fault'] != undefined)
	{
		var decode_fault = true;
		var fault_response = extra_options['return_on_fault'];
	}
	else
	{
		var decode_fault = false;
		var fault_response = '';
	}
	var debug = extra_options['debug'] != undefined ? (extra_options['debug']) : 0;
	var msgclass = prefix + 'msg';
	var valclass = prefix + 'val';
	var decodefunc = prefix + '_decode';

	var msg = new this[msgclass]('system.methodSignature'); // JS is Magik!!!
	msg.addParam(new this[valclass](methodname));
	client.setDebug(debug);
	var response = client.send(msg, timeout, protocol); // WARN: is user sent us a function as 'protocol', we're in deep shit
	if(response.faultCode())
	{
		xmlrpc_error_log('XML-RPC: could not retrieve method signature from remote server for method ' + methodname);
		return false;
	}
	else
	{
		var msig = response.value();
		if (client.return_type != 'jsvals')
		{
			msig = this[decodefunc](msig);
		}
		if( !(msig instanceof Array) || msig.length <= signum)
		{
			xmlrpc_error_log('XML-RPC: could not retrieve method signature nr.' + signum + ' from remote server for method ' + methodname);
			return false;
		}
		else
		{
			var msig = msig[signum];
			var mdesc = '';
			// if in 'offline' mode, get method description too.
			// in online mode, favour speed of operation
			if(buildit)
			{
				var xmlrpcfuncname = ''; // we build an anon func and return it back!
			}
			else
			{
				// pick a suitable name for the new function, avoiding collisions
				if(newfuncname != '')
				{
					var xmlrpcfuncname = newfuncname;
				}
				else
				{
					// take care to insure that methodname is translated to valid
					// php function name
					var xmlrpcfuncname = prefix + '_' + methodname.replace(/\./g, '_').replace(/[^a-zA-Z0-9_\x7f-\xff]/g, '');
				}
				/// @todo: how to find if a function exists, given its name as string?
				/// nb: it might not even be necessary, since we never do direct evaluation of it...
				/*while($buildit && function_exists($xmlrpcfuncname))
				{
					$xmlrpcfuncname .= 'x';
				}*/
				msg = new this[msgclass]('system.methodHelp');
				msg.addParam(new this[valclass](methodname));
				response = client.send(msg, timeout, protocol);
				if (!response.faultCode())
				{
					mdesc = response.value();
					if (client.return_type != 'jsvals')
					{
						mdesc = mdesc.scalarVal();
					}
				}
			}

			var results = build_remote_method_wrapper_code(client, methodname,
				xmlrpcfuncname, msig, mdesc, timeout, protocol, simple_client_copy,
				prefix, decode_js_objects, encode_js_objects, decode_fault,
				fault_response);
			//print_r($code);
			if (buildit)
			{
				var func = false;
				eval('func = ' + results['source']); //.'allOK=1;');
				// alternative
				//$xmlrpcfuncname = create_function('$m', $innercode);
				if(func)
				{
					return func;
				}
				else
				{
					xmlrpc_error_log('XML-RPC: could not create function ' + xmlrpcfuncname + ' to wrap remote method ' + methodname);
					return false;
				}
			}
			else
			{
				results['function'] = xmlrpcfuncname;
				return results;
			}
		}
	}
}

/**
* Similar to wrap_xmlrpc_method, but will generate a javascript class that wraps
* all xmlrpc methods exposed by the remote server as own methods.
* For more details see wrap_xmlrpc_method.
* @param xmlrpc_client client the client obj all set to query the desired server
* @param object extra_options list of options for wrapped code
* @return mixed false on error, the name of the created class if all ok or an array with code, class name and comments (if the appropriate option is set in extra_options)
* @access public
* @return void
*/
function wrap_xmlrpc_server(client, extra_options)
{
	if (extra_options === undefined) extra_options = {};
	var methodfilter = extra_options['method_filter'] !== undefined ? extra_options['method_filter'] : '';
	var signum = extra_options['signum'] !== undefined ? parseInt(extra_options['signum']) : 0;
	var timeout = extra_options['timeout'] !== undefined ? parseInt(extra_options['timeout']) : 0;
	var protocol = extra_options['protocol'] !== undefined ? extra_options['protocol'] : '';
	var newclassname = extra_options['new_class_name'] !== undefined ? extra_options['new_class_name'] : '';
	var encode_js_objects = extra_options['encode_js_objs'] !== undefined ? Boolean(extra_options['encode_js_objs']) : false;
	var decode_js_objects = extra_options['decode_js_objs'] !== undefined ? Boolena(extra_options['decode_js_objs']) : false;
	var verbatim_client_copy = extra_options['simple_client_copy'] !== undefined ? !Boolean(extra_options['simple_client_copy']) : true;
	var buildit = extra_options['return_source'] !== undefined ? !Boolean(extra_options['return_source']) : true;
	var prefix = extra_options['prefix'] !== undefined ? extra_options['prefix'] : 'xmlrpc';

	var msgclass = prefix + 'msg';
	//$valclass = $prefix.'val';
	var decodefunc = prefix + '_decode';

	var msg = new this[msgclass]('system.listMethods');
	var response = client.send(msg, timeout, protocol);
	if(response.faultCode())
	{
		xmlrpc_error_log('XML-RPC: could not retrieve method list from remote server');
		return false;
	}
	else
	{
		var mlist = response.value();
		if (client.return_type != 'jsvals')
		{
			mlist = this[decodefunc](mlist);
		}
		if(!(mlist instanceof Array) || !mlist.length)
		{
			xmlrpc_error_log('XML-RPC: could not retrieve meaningful method list from remote server');
			return false;
		}
		else
		{
			// pick a suitable name for the new function, avoiding collisions
			if(newclassname != '')
			{
				var xmlrpcclassname = newclassname;
			}
			else
			{
				var xmlrpcclassname = prefix + '_' + client.server.replace(/\./g, '_').replace(/[^a-zA-Z0-9_\x7f-\xff]/g, '') + '_client';
			}
		//while($buildit && class_exists($xmlrpcclassname))
		//{
		//	$xmlrpcclassname .= 'x';
		//}

			/// @todo add function setdebug() to new class, to enable/disable debugging
			var source = 'function ' + xmlrpcclassname +'()\n{\nvar client;\n\n';
			//source += 'function xmlrpcclassname()\n{\n';
			source += build_client_wrapper_code(client, verbatim_client_copy, prefix);
			source += 'this.client = \client;\n\n';
			var opts = {'simple_client_copy': 2, 'return_source': true,
				'timeout': timeout, 'protocol': protocol,
				'encode_js_objs': encode_js_objects, 'prefix': prefix,
				'decode_js_objs': decode_js_objects
			};
			/// @todo build javadoc for class definition, too

			for(var i = 0; i < mlist.length; i++)
			{
				var mname = mlist[i];
				if (methodfilter == '' || mname.search(methodfilter) != -1)
				{
					var new_function_name = mname.replace(/\./, '_').replace(/[^a-zA-Z0-9_\x7f-\xff]/,'');
					opts['new_function_name'] = ' ';
					var methodwrap = wrap_xmlrpc_method(client, mname, opts);
					if (methodwrap)
					{
						if (!buildit)
						{
							source += methodwrap['docstring'];
						}
						source += 'this.' + new_function_name + ' = '+ methodwrap['source'] + '\n';
					}
					else
					{
						xmlrpc_error_log('XML-RPC: will not create class method to wrap remote method ' + mname);
					}
				}
			}
			source += '}\n';
			if (buildit)
			{
				var func = false;
				eval('func = ' + source);
				// alternative
				//$xmlrpcfuncname = create_function('$m', $innercode);
				if(func)
				{
					return func;
				}
				else
				{
					xmlrpc_error_log('XML-RPC: could not create class ' + xmlrpcclassname + ' to wrap remote server ' + client.server);
					return false;
				}
			}
			else
			{
				return {'class': xmlrpcclassname, 'code': source, 'docstring': ''};
			}
		}
	}
}

/**
* Given the necessary info, build js code that creates a new function to
* invoke a remote xmlrpc method.
* Take care that no full checking of input parameters is done to ensure that
* valid js code is emitted.
* @private
*/
function build_remote_method_wrapper_code(client, methodname, xmlrpcfuncname,
	msig, mdesc, timeout, protocol, client_copy_mode, prefix,
	decode_js_objects, encode_js_objects, decode_fault,
	fault_response)
{
  //return {'source': '', 'docstring': '/** Automatic method stub generation yet to be implemented **/'};
	var code = 'function ' + xmlrpcfuncname + ' (';
	if (client_copy_mode < 2)
	{
		// client copy mode 0 or 1 == partial / full client copy in emitted code
		var innercode = build_client_wrapper_code(client, client_copy_mode, prefix);
		innercode += 'if (debug !== undefined) client.setDebug(debug);\n';
		var this_ = '';
	}
	else
	{
		// client copy mode 2 == no client copy in emitted code
		var innercode = '';
		var this_ = 'this.';
	}
	innercode += 'var msg = new ' + prefix + 'msg(\'' + methodname + '\');\n';

	if (mdesc != '')
	{
		// take care that js comment is not terminated unwillingly by method description
		mdesc = "/**\n* " + mdesc.replace(/\*\//g, '* /') + '\n';
	}
	else
	{
		mdesc = '/**\nFunction ' + xmlrpcfuncname + '\n';
	}

	// param parsing
	var plist = [];
	var pcount = msig.length;
	for(var i = 1; i < pcount; ++i)
	{
		plist[i-1] = 'p'+i;
		ptype = msig[i];
		if (ptype == 'i4' || ptype == 'int' || ptype == 'boolean' || ptype == 'double' ||
			ptype == 'string' || ptype == 'base64' || ptype == 'null')
		{
			// only build directly xmlrpcvals when type is known and scalar
			innercode += 'var p' + i + ' = new ' + prefix + 'val(p' + i +', \'' + ptype + '\');\n';
		}
		else
		{
			if (encode_js_objects)
			{
				innercode += 'var p' + i + ' = ' + prefix + '_encode(p' + i +', {\'encode_js_objs\': true};\n';
			}
			else
			{
				innercode += 'var p' + i + ' = ' + prefix + '_encode(p' + i +');\n';
			}
		}
		innercode += 'msg.addParam(p' + i + ');\n';
		mdesc += '* @param ' + xmlrpc_2_js_type(ptype) + ' p' + i + '\n';
	}
	if (client_copy_mode < 2)
	{
		plist[i-1] = 'debug';
		mdesc += '* @param int debug when 1 (or 2) will enable debugging of the underlying ' + prefix + ' call (defaults to 0)\n';
	}
	plist = plist.join(', ');
	mdesc += '* @return ' + xmlrpc_2_js_type(msig[0]) + ' (or an ' + prefix + 'resp obj instance if call fails)\n*/\n';

	innercode += 'var res = ' + this_ + 'client.send(msg, ' + timeout + ', \'' + protocol +'\');\n';
	if (decode_fault)
	{
		if (typeof(fault_response) == 'string' && (fault_response.indexOf('%faultCode%') != -1 || fault_response.indexOf('%faultString%') != -1))
		{
			var respcode = '\'' + fault_response.replace(/'/g, '\'\'').replace(/\%faultCode\%/g, '\' + res.faultCode() + \'').replace(/\%faultString\%/g, '\' + res.faultString() + \'') + '\'';
		}
		else
		{
			var respcode = var_export(fault_response, true);
		}
	}
	else
	{
		var respcode = 'res';
	}
	if (decode_js_objects)
	{
		innercode += 'if (res.faultCode()) return ' + respcode + '; else return ' + prefix + '_decode(res.value(), {\'decode_js_objs\': true});';
	}
	else
	{
		innercode += 'if (res.faultCode()) return ' + respcode + '; else return ' + prefix + '_decode(res.value());';
	}

	code = code + plist + ") {\n" + innercode + '\n}\n';

	return {'source' : code, 'docstring' : mdesc};
}

/**
* Given necessary info, generate js code that will rebuild a client object
* Take care that no full checking of input parameters is done to ensure that
* valid js code is emitted.
* @private
*/
function build_client_wrapper_code(client, verbatim_client_copy, prefix)
{
	var code = 'client = new ' + prefix +'_client(\'' + client.path.replace(/'/g, '\'') +
			'\', \'' + client.server.replace(/'/g, '\'') + '\', ' + client.port + ');\n';

	// copy all client fields to the client that will be generated runtime
	// (this provides for future expansion or subclassing of client obj)
	if (verbatim_client_copy)
	{
		for(var fld in client)
		{
			if(fld != 'debug' && fld != 'return_type' && typeof client[fld] != 'function')
			{
				val = var_export(client[fld], true);
				code += 'client.' + fld + ' = ' + val + ';\n';
			}
		}
		// only make sure that client always returns the correct data type
		code += 'client.return_type = \'' + prefix + 'vals\';\n';
	}
	//$code .= "\$client->setDebug(\$debug);\n";
	return code;
}