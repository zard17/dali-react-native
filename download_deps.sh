#!/bin/bash
set -e

DIR="third_party_dependencies"
mkdir -p $DIR

# Folly
FOLLY_VER="2024.11.18.00"
echo "Downloading Folly $FOLLY_VER..."
if [ ! -d "$DIR/folly" ]; then
    # Clone is heavy, try tarball if available. GitHub releases tags are usually vYYYY.MM.DD.00
    curl -L "https://github.com/facebook/folly/archive/refs/tags/v${FOLLY_VER}.tar.gz" -o folly.tar.gz
    tar -xf folly.tar.gz
    mv "folly-${FOLLY_VER}" "$DIR/folly"
    rm folly.tar.gz
fi

# Generate folly-config.h
echo "Generating folly-config.h..."
FOLLY_CONFIG_H="$DIR/folly/folly/folly-config.h"
cat <<EOF > "$FOLLY_CONFIG_H"
#pragma once

#define FOLLY_MOBILE 1
#define FOLLY_USE_LIBCPP 1
#define FOLLY_HAVE_PTHREAD 1
#define FOLLY_CFG_NO_COROUTINES 1
#define FOLLY_HAVE_CLOCK_GETTIME 1

#pragma clang diagnostic ignored "-Wcomma"
EOF

# Double Conversion (Dependency of Folly header sometimes)
echo "Downloading Double Conversion..."
if [ ! -d "$DIR/double-conversion" ]; then
   git clone https://github.com/google/double-conversion.git "$DIR/double-conversion"
fi

# Glog
echo "Downloading Glog..."
if [ ! -d "$DIR/glog" ]; then
   git clone https://github.com/google/glog.git "$DIR/glog"
fi

# Fmt
echo "Downloading Fmt..."
if [ ! -d "$DIR/fmt" ]; then
   git clone https://github.com/fmtlib/fmt.git "$DIR/fmt"
fi


# FastFloat
echo "Downloading FastFloat..."
if [ ! -d "$DIR/fast_float" ]; then
   git clone https://github.com/fastfloat/fast_float.git "$DIR/fast_float"
fi


# Boost 1.83.0
BOOST_VER="1_83_0"
BOOST_DOT_VER="1.83.0"
echo "Downloading Boost ${BOOST_DOT_VER}..."
if [ ! -d "$DIR/boost_${BOOST_VER}" ]; then
    # Use valid URL
    curl -L "https://archives.boost.io/release/${BOOST_DOT_VER}/source/boost_${BOOST_VER}.tar.gz" -o boost.tar.gz
    # This might fail if the URL is wrong, checking status
    if [ $? -ne 0 ]; then
        echo "Failed to download boost."
        exit 1
    fi
    tar -xf boost.tar.gz -C "$DIR"
    rm boost.tar.gz
fi

# LibEvent
echo "Downloading LibEvent..."
if [ ! -d "$DIR/libevent" ]; then
    curl -L "https://github.com/libevent/libevent/releases/download/release-2.1.12-stable/libevent-2.1.12-stable.tar.gz" -o libevent.tar.gz
    if [ $? -ne 0 ]; then
        echo "Failed to download libevent."
        exit 1
    fi
    tar -xf libevent.tar.gz
    mv libevent-2.1.12-stable "$DIR/libevent"
    rm libevent.tar.gz
fi


