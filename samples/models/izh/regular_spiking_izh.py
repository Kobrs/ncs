#!/usr/bin/python

import sys

import ncs

def run(argv):
	sim = ncs.Simulation()
	regular_spiking_parameters = sim.addModelParameters("regular_spiking","izhikevich",
								{
								 "a": 0.02,
								 "b": 0.2,
								 "c": -65.0,
								 "d": 8.0,
								 "u": -12.0,
								 "v": -60.0,
								 "threshold": 30,
								})
	group_1=sim.addCellGroup("group_1",1,regular_spiking_parameters,None)
	if not sim.init(argv):
		print "failed to initialize simulation."
		return

	sim.addInput("rectangular_current",{"amplitude":10,"width": 1, "frequency": 1},group_1,1,0.01,1.0)
	voltage_report=sim.addReport("group_1","neuron","neuron_voltage",1.0)
	voltage_report.toAsciiFile("./regular_spiking.txt")	
	
	sim.step(1000)

	return

if __name__ == "__main__":
	run(sys.argv)
