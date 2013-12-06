#!/usr/bin/python

import sys

import ncs

def run(argv):
	sim = ncs.Simulation()
	phasic_spiking_parameters = sim.addModelParameters("phasic_spiking","izhikevich",
								{
								 "a": 0.02,
								 "b": 0.25,
								 "c": -65.0,
								 "d": 6.0,
								 "u": -16.25,
								 "v": -65.0,
								 "threshold": 30,
								})
	group_1=sim.addCellGroup("group_1",1,phasic_spiking_parameters,None)
	if not sim.init(argv):
		print "failed to initialize simulation."
		return

	sim.addInput("rectangular_current",{"amplitude":0.5,"width": 1, "frequency": 1},group_1,1,0.01,1.0)
	voltage_report=sim.addReport("group_1","neuron","neuron_voltage",1.0)
	voltage_report.toAsciiFile("./phasic_spiking_izh.txt")	
	sim.step(1000)

	return

if __name__ == "__main__":
	run(sys.argv)
