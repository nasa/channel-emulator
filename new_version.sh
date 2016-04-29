#!/bin/bash
AUTHOR_NAME='Tad Kollar'
AUTHOR_MAIL='< >'

NEW_VERSION=$(cat version.txt)
CHANGE_LIST=$(egrep -v '^ *#' current_changes.txt)
DATE=$(printf "%(%F)T" -1)
DEBIAN_VERSION='precise'

LINE='-----------------------------------------------------------------------'
LINE_WRAP=72

asterisks=$(echo "$CHANGE_LIST"|sed -e 's/^/  * /')
wrapped_asterisks=$(echo "$CHANGE_LIST"|sed -e 's/^/* /'|fold -s -w $LINE_WRAP)
hyphens=$(echo "$CHANGE_LIST"|perl -pe "chomp; s/^/--/; s/\$/\\\n/; s/([\"\'\{\}])/\\\\\$1/g;")

#--------------------------------------
# Update CMakeLists.txt
#--------------------------------------
cmakelists='./CMakeLists.txt'
MVERS=( ${NEW_VERSION//[.]/ } )
perl -i.bkp -pe "s/CTSCE_VERSION_MAJOR [0-9]+/CTSCE_VERSION_MAJOR ${MVERS[0]}/; s/CTSCE_VERSION_MINOR [0-9]+/CTSCE_VERSION_MINOR ${MVERS[1]}/; s/CTSCE_VERSION_PATCH [0-9]+/CTSCE_VERSION_PATCH ${MVERS[2]}/; s/CTSCE_VERSION_TWEAK [0-9]+/CTSCE_VERSION_TWEAK ${MVERS[3]}/;" $cmakelists && {
	rm ${cmakelists}.bkp
}

#--------------------------------------
# Update top ChangeLog
#--------------------------------------
if [ ! -f ChangeLog ]; then
	echo "ChangeLog not found, wrong directory?"
	exit 1
fi

mv ChangeLog ChangeLog.old
echo "$LINE" > ChangeLog
echo "Version: $NEW_VERSION" >> ChangeLog
echo "Date: $DATE" >> ChangeLog
echo "$LINE" >> ChangeLog
echo "$wrapped_asterisks" >> ChangeLog
echo "$LINE" >> ChangeLog
echo >> ChangeLog
cat ChangeLog.old >> ChangeLog
rm ChangeLog.old

#--------------------------------------
# Update Debian changelog
#--------------------------------------
debch='debian/changelog'
debtime=$(printf "%(%a, %d %b %Y %T %z)T" -1)
if [ ! -f $debch ]; then
	echo "$debch not found, wrong directory?"
	exit 1
fi

mv $debch ${debch}.old
echo "ctsce (${NEW_VERSION}) ${DEBIAN_VERSION}; urgency=low" > $debch
echo "" >> $debch
echo "$asterisks" >> $debch
echo "" >> $debch
echo " -- ${AUTHOR_NAME} <${AUTHOR_MAIL}>  ${debtime}" >> $debch
echo "" >> $debch
cat ${debch}.old >> $debch
rm ${debch}.old

#--------------------------------------
# Update rpmbuild ctsce.spec
#--------------------------------------
rpmspec='rpmbuild/ctsce.spec'
rpmtime=$(printf "*%(%a %b %d %Y)T %s <%s> %s" -1 "$AUTHOR_NAME" "$AUTHOR_MAIL" "$NEW_VERSION")

if [ ! -f $rpmspec ]; then
	echo "$rpmspec not found, wrong directory?"
	exit 1
fi

perl -i.bkp -pe "s/^Version:\s+\S+.*\$/Version:        ${NEW_VERSION}/; if ( /^%changelog/ ) { \$_ = ''; print '%changelog',\"\n\",'${rpmtime}',\"\n${hyphens}\n\"; }" $rpmspec && {
	rm ${rpmspec}.bkp
}

#--------------------------------------
# Update www/index.html
#--------------------------------------
idxhtml='www/index.html'

perl -i.bkp -pe "s/<h3>Version [0-9\.]+<\/h3>/<h3>Version ${NEW_VERSION}<\/h3>/;" $idxhtml && {
	rm ${idxhtml}.bkp
}
