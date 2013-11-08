#!/usr/bin/python

import math
import sys

import ncs

def Run(argv):
  ncs_cell = {
    "threshold": -50.0,
    "resting_potential": ncs.Uniform(-62.0, -58.0),
    "calcium": 0.0,
    "calcium_spike_increment": 0.7,
    "tau_calcium": 0.02,
    "leak_reversal_potential": 0.0,
    "tau_membrane": 0.02,
    "r_membrane": 200.0,
    "spike_shape": [
      -38, 30, -43, -60, -60
    ],
    "capacitance": 1.0,
    "channels": [
    ]
  }

  sim = ncs.Simulation()
  neuron_parameters = sim.addModelParameters("ncs_neuron",
                                             "ncs",
                                             ncs_cell
                                            )
  group_1 = sim.addCellGroup("group_1", 50, "ncs_neuron", None) # last param is geometry

  all_cells = sim.addCellAlias("all_cells", [group_1])
  sim.addCellAlias("all", all_cells)
  sim.addCellAlias("all_2", "all_cells")

  if not sim.init(argv):
    print "Failed to initialize simulation."
    return

  sim.addInput("rectangular_current", { "amplitude": 0.1 }, group_1, 0.5, 0.0, 1.0)

  voltage_report = sim.addReport("group_1", "neuron", "neuron_voltage", 1.0)
  voltage_report.toAsciiFile("/tmp/voltages.txt")
#voltage_report.toStdOut()

  sim.step(500)
  return

if __name__ == "__main__":
  Run(sys.argv)


