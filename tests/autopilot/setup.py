#!/usr/bin/python

# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.


from distutils.core import setup
from setuptools import find_packages

setup(
    name='ubuntu-system-settings',
    version='0.1',
    description='Ubuntu System Settings autopilot tests.',
    url='https://launchpad.net/ubuntu-system-settings',
    license='GPLv3',
    packages=find_packages(),
)
