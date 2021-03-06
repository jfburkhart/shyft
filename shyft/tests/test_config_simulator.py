from os import path
import unittest

from shyft import shyftdata_dir
from shyft.repository.default_state_repository import DefaultStateRepository
from shyft.orchestration.configuration.yaml_configs import YAMLSimConfig
from shyft.orchestration.configuration.yaml_configs import YAMLCalibConfig
from shyft.orchestration.simulators.config_simulator import ConfigSimulator
from shyft.orchestration.simulators.config_simulator import ConfigCalibrator

from shyft.api import IntVector, Calendar
from shyft.orchestration.configuration.yaml_configs import utctime_from_datetime
import datetime as dt


class ConfigSimulationTestCase(unittest.TestCase):
    def test_utctime_from_datetime(self):
        utc = Calendar()
        dt1 = dt.datetime(2015, 6, 1, 2, 3, 4)
        t1 = utctime_from_datetime(dt1)
        self.assertEqual(t1, utc.time(2015, 6, 1, 2, 3, 4))

    def test_run_geo_ts_data_config_simulator(self):
        # These config files are versioned in shyft-data git
        config_dir = path.join(shyftdata_dir, "neanidelv", "yaml_config")
        config_file = path.join(config_dir, "neanidelva_simulation.yaml")
        config_section = "neanidelva"
        cfg = YAMLSimConfig(config_file, config_section, overrides={'config': {'number_of_steps': 168}})

        # These config files are versioned in shyft-data git. Read from ${SHYFTDATA}/netcdf/orchestration-testdata/
        simulator = ConfigSimulator(cfg)

        # Regression tests on interpolation parameters
        self.assertAlmostEqual(simulator.region_model.interpolation_parameter.precipitation.scale_factor, 1.01, 3)

        #n_cells = simulator.region_model.size()
        state_repos = DefaultStateRepository(simulator.region_model)
        simulator.run(cfg.time_axis, state_repos.get_state(0))
        cids = IntVector()
        discharge = simulator.region_model.statistics.discharge(cids)

        # Regression tests on discharge values
        self.assertAlmostEqual(discharge.values[0], 0.1000929, 3)
        self.assertAlmostEqual(discharge.values[3], 3.9918816, 3)
        # Regression tests on geo fractions
        self.assertAlmostEqual(simulator.region_model.cells[0].geo.land_type_fractions_info().unspecified(), 1.0, 3)
        self.assertAlmostEqual(simulator.region_model.cells[2].geo.land_type_fractions_info().unspecified(), 0.1433, 3)
        self.assertAlmostEqual(simulator.region_model.cells[2].geo.land_type_fractions_info().forest(), 0.0, 3)
        self.assertAlmostEqual(simulator.region_model.cells[2].geo.land_type_fractions_info().reservoir(), 0.8566, 3)

    def test_run_geo_ts_data_config_calibrator(self):
        # These config files are versioned in shyft git
        config_dir = path.join(shyftdata_dir,"neanidelv","yaml_config")
        config_file = path.join(config_dir, "neanidelva_calibration.yaml")
        print(config_file)
        config_section = "neanidelva"
        cfg = YAMLCalibConfig(config_file, config_section)
        calibrator = ConfigCalibrator(cfg)

        # Regression tests on interpolation parameters
        self.assertAlmostEqual(calibrator.region_model.interpolation_parameter.precipitation.scale_factor, 1.01, 3)

if __name__ == '__main__':
    unittest.main()
