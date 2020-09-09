#!/bin/bash

XDS_SRC_DIR="$(cd "$(dirname "$0")";pwd)/../.."

if [[ -z "$1" ]]
then
  XDS_BUILD_DIR="."
else
  XDS_BUILD_DIR="$1"
fi

#------------------------------------------------------------------------------
# PROCEDURE RemoveFiles (new_dir_name);
# @return: 0 on success, error code otherwise
MakeBuildDir()
{
  local new_dir_name="$XDS_BUILD_DIR/${1}"
  if [ ! -e "$new_dir_name" ]
  then
    mkdir -p "$new_dir_name"
    return $?
  fi
}

#------------------------------------------------------------------------------
# PROCEDURE CopyDir (src_dir dest_dir);
# @return: 0 on success, error code otherwise
CopyDir()
{
  local src_dir="$XDS_SRC_DIR/${1}"       # source directory
  local dest_dir="$XDS_BUILD_DIR/${2}"    # destination directory
  echo "Copy $src_dir  ->  $dest_dir"

  if [ ! -e "$dest_dir" ]
  then
    mkdir -p "$dest_dir"
  fi

  # -a --archive, the same as -dR --preserv=all
  # -T --no_target_directory, treat dest as a normal file
  cp -aT "$src_dir"  "$dest_dir"
  return $?
}

#------------------------------------------------------------------------------
# PROCEDURE CopyDir (files_to_copy dest_dir);
CopyFiles()
{
  local files_to_copy="$XDS_SRC_DIR/${1}"   # files to be copied
  local dest_dir="$XDS_BUILD_DIR/${2}"      # destination directory
  local exit_code=0

  if [ ! -e "$dest_dir" ]
  then
    mkdir -p "$dest_dir"
  fi

  for file_name in $files_to_copy
  do
    # -p preserv mode,ownership,timestamp
    # -H follow command line symbolic link
    cp -H -f --remove-destination "$file_name"  "$dest_dir"  ||  exit_code=$?
  done

  return $exit_code
}


#------------------------------------------------------------------------------
# PROCEDURE RemoveFiles (files_to_remove);
RemoveFiles()
{
  local files_to_remove="$XDS_BUILD_DIR/${1}"  # files to be removed
  local exit_code=0

  for file_name in $files_to_remove
  do
    if [ -e "$file_name" ]
    then
      rm -rf "$file_name" >/dev/null 2>&1 || exit_code=$?
    fi
  done

  return $exit_code
}


# ============================================= Unpack Binary version of XDS for Linux
if [ ! -e "../../bin/xds" ]
then
  echo "Unpack binray version of XDS 2.60"
  pushd "../../bin"
  tar -xvf xds-260-linux.tgz >/dev/null 2>&1  || exit $? 
  popd
fi 


# ============================================= Create Folders 
. ./setenv.sh

MakeBuildDir "src"  || exit $?

MakeBuildDir "enduser/xds/bin"   || exit $?

MakeBuildDir "enduser/xds/C/C"           || exit $?
MakeBuildDir "enduser/xds/C/x86"         || exit $?
MakeBuildDir "enduser/xds/include/C"     || exit $?
MakeBuildDir "enduser/xds/include/x86"   || exit $?
MakeBuildDir "enduser/xds/lib/C"         || exit $?
MakeBuildDir "enduser/xds/lib/x86"       || exit $?
MakeBuildDir "enduser/xds/sym/C"         || exit $?
MakeBuildDir "enduser/xds/sym/x86"       || exit $?

MakeBuildDir "enduser/xds/def/iso"       || exit $?
MakeBuildDir "enduser/xds/def/ob2"       || exit $?
MakeBuildDir "enduser/xds/def/pim"       || exit $?
MakeBuildDir "enduser/xds/def/POSIX"     || exit $?
MakeBuildDir "enduser/xds/def/ts"        || exit $?
MakeBuildDir "enduser/xds/def/X11"       || exit $?
MakeBuildDir "enduser/xds/def/xds"       || exit $?
MakeBuildDir "enduser/xds/def/Xm"        || exit $?

MakeBuildDir "enduser/xds/doc"           || exit $?
MakeBuildDir "enduser/xds/pdf"           || exit $?
MakeBuildDir "enduser/xds/readme"        || exit $?

MakeBuildDir "enduser/xds/licenses"      || exit $?

for file_name in build.sh makexds-c.sh makexds-x86.sh pack-xds.sh setenv.sh
do
  chmod a+x "$file_name"
done

if [ "$XDS_BUILD_DIR" != "." ]
then
  for file_name in build.sh makexds-c.sh makexds-x86.sh pack-xds.sh setenv.sh
  do
    CopyFiles "build/x86Linux/$file_name"  ""  || exit $?
  done

  MakeBuildDir "bin"  || exit $?
  ln -s -f "$XDSDIR" "$XDS_BUILD_DIR/bin/xds"  || exit $?
fi 

# ============================================= Compiler
CopyDir   "comp/src"                    "src/"  || exit $?     

CopyFiles "comp/enduser/share/bin/*"  "enduser/xds/bin/"  || exit $?
CopyFiles "comp/enduser/linux/bin/*"  "enduser/xds/bin/"  || exit $?

# ============================================= API 
CopyFiles "api/enduser/linux/*"  "enduser/xds/"  || exit $?

CopyDir "api/src/X11"     "src/X11/"    || exit $?      
CopyDir "api/src/POSIX"   "src/POSIX/"  || exit $?      
CopyDir "api/src/Linux"   "src/Linux/"  || exit $?      

# ============================================= Library 
CopyDir "lib/src"  "src/"  || exit $?      

CopyFiles "lib/src/TSlibs/*.def"  "enduser/xds/def/ts/"  || exit $?
for def_file in MsMouse.def Graph.def
do
  RemoveFiles "enduser/xds/def/ts/$def_file"
done
RemoveFiles "enduser/xds/def/ts/x*.def"

# ============================================= Utils
CopyDir   "utils/h2d/src"          "src/h2d/"          || exit $?    
CopyFiles "utils/h2d/src/h2d.msg"  "enduser/xds/bin/"  || exit $?
CopyFiles "utils/h2d/src/h2d.cfg"  "enduser/xds/bin/"  || exit $?

CopyDir "utils/xlib/src"   "src/xlib/"   || exit $?      
CopyDir "utils/xlink/src"  "src/xlink/"  || exit $?      

# ============================================= Licence files  
CopyDir "misc/licenses/linux"  "enduser/xds/licenses/"  || exit $?

# ============================================= Readme files 
CopyDir "misc/readme/Common"    "enduser/xds/readme/"  || exit $?      
CopyDir "misc/readme/x86Linux"  "enduser/xds/readme/"  || exit $?      

# ============================================= Samples 
CopyDir "misc/Samples/Common"  "enduser/xds/samples/"      || exit $?      
CopyDir "misc/Samples/h2d"     "enduser/xds/samples/h2d/"  || exit $?      
CopyDir "misc/Samples/Native"  "enduser/xds/samples/"      || exit $?      

CopyFiles "misc/Samples/xdssamp"   "enduser/xds/bin/"      || exit $?      
CopyFiles "misc/Samples/buildc"    "enduser/xds/samples/"  || exit $?      
CopyFiles "misc/Samples/buildx86"  "enduser/xds/samples/"  || exit $?      

# ============================================= Fill "xds/doc" 
# NOTE: Help generation disabled 
# Prebuilt pdf documentation is provided
CopyFiles "misc/Doc/enduser/share/pdf/*.pdf"  "enduser/xds/pdf/"  || exit $?
CopyFiles "misc/Doc/enduser/linux/pdf/*.pdf"  "enduser/xds/pdf/"  || exit $?

echo "=== Ok ===  Creation of process is successfully complete"
echo "Run './build.sh' in \"$XDS_BUILD_DIR\""
