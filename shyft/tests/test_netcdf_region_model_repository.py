import unittest
from os import path
from shyft import shyftdata_dir
from shyft.repository.netcdf.region_model import RegionModelRepository
from shyft.repository import yaml_config
from shyft.api.pt_gs_k import PTGSKModel


class NetCDFRegionModelRepositoryTestCase(unittest.TestCase):
    """ Verify that yaml-based config, with netcdf data
        can provide ready made shyft models extracted from
        configuration at region-model level, (parameters)
        cell properties (geo_cell_data, mid_point, elevation etc..)
    """
    def test_construct_repository(self):
        reg_conf = yaml_config.RegionConfig(path.join(path.dirname(__file__),
                                            "netcdf", "region.yaml"))
        mod_conf = yaml_config.ModelConfig(path.join(path.dirname(__file__),
                                           "netcdf", "model.yaml"))
        region_model_repository = RegionModelRepository(reg_conf, mod_conf)
        self.assertIsNotNone(region_model_repository.mask)
        region_model = \
            region_model_repository.get_region_model("NeaNidelv_PTGSK",
                                                     PTGSKModel)
        self.assertIsNotNone(region_model)
        self.assertEquals(4960, region_model.size())
        rp = region_model.get_region_parameter()
        self.assertAlmostEquals(-2.63036759414, rp.kirchner.c1)
        self.assertTrue(
            region_model.has_catchment_parameter(1),
            "There is a catchment override in the region.yaml file")
        c1p = region_model.get_catchment_parameter(1)
        self.assertAlmostEquals(-2.539, c1p.kirchner.c1)
