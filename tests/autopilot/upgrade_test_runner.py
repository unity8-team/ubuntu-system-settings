#!/usr/bin/python3

import argparse
import sys
import unittest

from system_upgrade.helpers.system_image_flasher import DeviceImageFlash
from system_upgrade.variables import (
    PACKAGE_LIST,
    TEST1_PART1,
    TEST1_PART2,
)


class SystemImageUpgrader(unittest.TestCase, DeviceImageFlash):

    log_path = '/tmp/upgrade.log'
    source = 'system_upgrade'
    target_home = '/home/phablet/'
    test_suite = 'system_upgrade'
    device_test_path = '/home/phablet/{}'.format(test_suite)
    unlocker = 'unlock_screen.py'
    unlocker_abs_location = target_home + unlocker
    unlocker_location = 'system_upgrade/helpers/' + unlocker

    def setUp(self):
        self.dut_serial = arguments.serial
        self.extra_packages = arguments.packages
        self.setup_device(
            ppa=arguments.ppa,
            package_list=PACKAGE_LIST,
            bootstrap=True,
            revision_number='-1'
        )

    def test_system_update(self):
        self._push_and_run_tests(test_suite=TEST1_PART1)
        log_file = self.open_log_file()
        self.assertTrue(log_file.endswith('OK\n'))

        self.wait_for_device(timeout=150)

        self._push_and_run_tests(test_suite=TEST1_PART2)
        log_file = self.open_log_file()
        self.assertTrue(log_file.endswith('OK\n'))

    def open_log_file(self):
        log_file = open(self.log_path, 'r')
        self.addCleanup(log_file.close)
        return log_file.read()

    def _push_and_run_tests(self, test_suite):
        self.push_files_to_device(self.source, self.device_test_path)
        self.push_files_to_device(
            'ubuntu_system_settings', '/home/phablet/ubuntu_system_settings'
        )
        self.unlock_device_screen()
        self.run_autopilot_test_on_target(test_suite)

    def push_files_to_device(self, source, location):
        self.addCleanup(self.delete_autopilot_test)
        command = 'adb -s {} push {} {}'.format(
            self.dut_serial, source, location
        )
        self.run_command_on_host(command)

    def delete_autopilot_test(self):
        command = 'rm -rf {}'.format(self.device_test_path)
        self.run_command_on_target(command)

    def unlock_device_screen(self):
        self.push_files_to_device(self.unlocker_location, self.target_home)
        self.run_command_on_target(
            'chmod +x {}'.format(self.unlocker_abs_location)
        )
        self.run_command_on_target('./unlock_screen.py', user='phablet')


def _parse_command_line_arguments():
    parser = argparse.ArgumentParser(
        description='Ubuntu system update test runner'
    )
    parser.add_argument(
        'serial', metavar='SERIAL_NUMBER', type=str,
        help='Serial numer of the device to run tests on.'
    )
    parser.add_argument(
        '--ppa', type=str,
        help='Launchpad ppa to add to the device before installing '
             'test packages.'
    )
    parser.add_argument(
        '--packages', type=str,
        help='Names of extra packages to be installed on the test device '
             'before running the tests.'
    )

    return parser.parse_args()


if __name__ == '__main__':
    arguments = _parse_command_line_arguments()
    unittest.main(argv=[sys.argv[0]])
