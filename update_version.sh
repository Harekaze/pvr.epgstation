#!/bin/sh

if [ $# -lt 1 ]; then
  echo usage: $0 '<version>'
  exit 1
fi

echo Update version to $1

echo Check changelog
head -1 ChangeLog.txt | grep -q $1
if [ ! $? -eq 0 ]; then
  echo No changelog found.
  exit 1
fi

xmllint --noout --shell template/pvr.epgstation/addon.xml > /dev/null << EOF
cd //addon/@version
set $1
save
EOF

awk -F ', ' '/^AC_INIT/ {sub("\\"$2, "['$1']")}; { print }' configure.ac > configure.ac.new
cat configure.ac.new > configure.ac
rm configure.ac.new

iconv -f UTF-16LE VS2019/pvr_client/pvr_client.rc | awk '
/(FILE|PRODUCT)VERSION/ {sub(/([0-9]+,){3}/, "'$(tr . , <<< $1)',")};
/(File|Product)Version/ {sub(/([0-9]+\.){3}/, "'$1'.")};
{ print }' > VS2019/pvr_client/pvr_client.rc.new
iconv -t UTF-16LE VS2019/pvr_client/pvr_client.rc.new > VS2019/pvr_client/pvr_client.rc
rm VS2019/pvr_client/pvr_client.rc.new

git commit -am ":up: Release v$1"
git tag v$1

echo done.