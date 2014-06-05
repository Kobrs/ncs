#!/usr/bin/python

import os, sys
ncs_lib_path = ('../../../')
sys.path.append(ncs_lib_path)
import ncs

def run(argv):
  sim = ncs.Simulation()
  excitatory_parameters = sim.addNeuron("label_excitatory",
                                                 "izhikevich",
                                                 { 
                                                  "a": 0.2,
                                                  "b": 0.2,
                                                  "c": -65.0,
                                                  "d": ncs.Uniform(7.0, 9.0),
                                                  "u": ncs.Uniform(-15.0, -11.0),
                                                  "v": ncs.Normal(-60.0, 5.0),
                                                  "threshold": 30,
                                                 }
                                                )
  group_1 = sim.addNeuronGroup("group_1", 100, "label_excitatory", None) # last param is geometry
  group_2 = sim.addNeuronGroup("group_2", 100, excitatory_parameters)

  all_cells = sim.addNeuronAlias("all_cells", [group_1, "group_2"])
  sim.addNeuronAlias("all", all_cells)
  sim.addNeuronAlias("all_2", "all_cells")

  flat_parameters = sim.addSynapse("flat_synapse", 
                                           "flat", 
                                           { "delay": 2,
                                             "current": ncs.Normal(18.0,2.0)
                                           })
  all_to_all = sim.addSynapseGroup("all_to_all", all_cells, "all_2", 0.1, flat_parameters)
  all_to_all_2 = sim.addSynapseGroup("all_to_all_2", 
                             [group_1, group_2], 
                             "all_2", 
                             0.1, 
                             flat_parameters)
  one_to_two = sim.addSynapseGroup("one_to_two", 
                           group_1, 
                           "group_2", 
                           0.1, 
                           "flat_synapse")

  all_connections = sim.addSynapseAlias("all_connections", [all_to_all, one_to_two])

  if not sim.init(argv):
    print "Failed to initialize simulation."
    return

  sim.addStimulus("rectangular_current", { "amplitude": 18.0 }, group_1, 0.1, 0.0, 1.0)
  sim.addStimulus("rectangular_current", { "amplitude": 18.0 }, "all", 0.1, 0.0, 0.5)
  sim.addStimulus("rectangular_current", { "amplitude": 18.0 }, "group_2", 0.1, 0.1, 1.1)

  voltage_report = sim.addReport("group_1", "neuron", "neuron_voltage", 1.0,0.0,0.01)
  voltage_report.toAsciiFile("/tmp/foo.txt")

  fire_report = sim.addReport(all_cells, "neuron", "neuron_fire", 1.0,0.0,0.01)
  fire_report.toStdOut()

  
  sim.run(duration=0.01)
  return

if __name__ == "__main__":
  run(sys.argv)
