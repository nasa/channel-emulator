export CMD=/usr/bin/xmlrpc
export URL='http://localhost:8080/RPC2'

function ce_run() {
	echo "Running '$*'"
	$CMD $URL $*
	if [ $? -gt 0 ]; then
		echo "Call failed, exiting."
		exit 1
	fi
}
