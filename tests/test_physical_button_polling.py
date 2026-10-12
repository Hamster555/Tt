"""Regression guards for the physical-button/web-command lifecycle.

These are source-level checks, not hardware or runtime safety tests.
"""
from pathlib import Path
import unittest


SOURCE = Path("src/main.cpp").read_text(encoding="utf-8")


class PhysicalButtonPollingTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        start = SOURCE.index("void handlePhysicalButtons() {")
        end = SOURCE.index("\nvoid setup() {", start)
        cls.handler = SOURCE[start:end]

    def test_release_tracking_is_per_actuator(self):
        self.assertIn("static bool prevLeftAny = false;", self.handler)
        self.assertIn("static bool prevRightAny = false;", self.handler)
        self.assertIn("else if (prevLeftAny)", self.handler)
        self.assertIn("else if (prevRightAny)", self.handler)

    def test_unpressed_buttons_do_not_unconditionally_stop_web_commands(self):
        self.assertNotIn("else stopActuator(leftTab);", self.handler)
        self.assertNotIn("else stopActuator(rightTab);", self.handler)

    def test_physical_release_still_stops_the_actuator_it_controlled(self):
        self.assertIn("else if (prevLeftAny) {\n      stopActuator(leftTab);", self.handler)
        self.assertIn("else if (prevRightAny) {\n      stopActuator(rightTab);", self.handler)


if __name__ == "__main__":
    unittest.main()
