#!/bin/sh
# Run in an Ubuntu 26.04 container as root. Version changes require visual review.
set -eu
apt-get update
DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
  build-essential pkg-config python3 python3-pil xvfb xauth fontconfig dbus-x11 \
  libgtk-4-dev=4.22.4+ds-0ubuntu0.1 \
  libpango1.0-dev=1.57.0-1 \
  libcairo2-dev=1.18.4-3 \
  libfreetype-dev=2.14.2+dfsg-1ubuntu0.1 \
  libfontconfig-dev=2.17.1-3ubuntu1 \
  fonts-dejavu-core=2.37-8build1
