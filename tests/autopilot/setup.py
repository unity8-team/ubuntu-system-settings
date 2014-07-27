#!/usr/bin/python3

# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013, 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.


import setuptools


setuptools.setup(
    name='ubuntu-system-settings',
    version='0.1',
    description='Ubuntu System Settings autopilot tests.',
    url='https://launchpad.net/ubuntu-system-settings',
    license='GPLv3',
    packages=setuptools.find_packages(),
    package_dir={
        'ubuntu_system_settings': './ubuntu_system_settings'},
    package_data={
        'ubuntu_system_settings': ['background_images/*.jpg'],
    }
)
