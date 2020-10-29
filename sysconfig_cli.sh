#!/bin/sh
#
# Run the SysConfig command line tool
#
# For help, use -h or --help 

DIR=`dirname "$0"`

# extract options to nodejs (if present)
nodeFlags=""
if [ "$1" = "-g" ]; then
    shift
    nodeFlags="$nodeFlags --inspect --debug-brk"
fi

CCS_NODE="$DIR/../../tools/node/node"
STANDALONE_NODE="$DIR/nodejs/node"

# try to use standalone node, then CCS node, finally any node version availiable on the path
if [ -f "$STANDALONE_NODE" ]; then
	NODE="$STANDALONE_NODE"
elif [ -f $CCS_NODE ]; then
	NODE="$CCS_NODE"
else 
	NODE=node
fi

# run the sys-config nodejs CLI and capture exit status
"$NODE" $nodeFlags "$DIR/dist/cli.js" "$@"