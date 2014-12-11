/*********************************************
 * OPL 12.5 Model
 * Author: pedro
 * Creation Date: Nov 13, 2014 at 3:09:53 PM
 *********************************************/

// Number of instructions
int nInstructions = ...;

// Number of processors
int p = ...;

range INS = 1..nInstructions;
range INSV = 2..nInstructions;

// Instruction dependency
// E(i,j) denotes that j depends on i
int E[i in INS][j in INS] = ...;


float tdf[i in INS] = ...;
float tex[i in INS] = ...;
float twb[i in INS] = ...;

// Upper bound
float U;

// Preprocessing U
execute {
	U = 0;
	for (var i = 1; i <= nInstructions; i++)
		U += tdf[i]+tex[i]+twb[i];
	//writeln("Value U: "+ U +".");
	//U = 1000000;
};

// Schedule graph, node i precedes node j
dvar boolean xij[i in INS][j in INS];

// Start time of instruction i
dvar int+ ti[i in INS];

// yij = xij*ti
dvar int+ yij[i in INS][j in INS];

// Makespan
dvar int+ z;

// Objective
minimize z;

subject to {
	// First node can have p sebsequent nodes at most (one per processor)
	sum (j in INS) xij[1][j] <= p;
	
	// First node can have noone as a subsequent
	forall(i in INS)
		xij[i][1] == 0;
	
	// An intermediate node can have at most one subsequent
	forall(i in INSV)
		sum (j in INS) xij[i][j] <= 1;
	
	// An intermediate node has exactly one predecessor
	forall(i in INSV)
		sum (j in INS) xij[j][i] == 1;
	
	// An intermediate node can not depend on himself
	forall(i in INSV)
		xij[i][i] == 0;
	
	// A subsequent node can not start the FD until
	// its predecessor has finished the FD
	forall (j in INS)
		sum (i in INS) (yij[i][j] + tdf[i]*xij[i][j]) <= ti[j];
	
	// A subsequent node can not start the EX until
	// its predecessor has finished the EX 
	forall (j in INS)
		sum (i in INS) (yij[i][j] + (tdf[i]+tex[i])*xij[i][j]) <= ti[j] + tdf[j];
	
	// A subsequent node can not start the WB until
	// its predecessor has finished the WB 
	forall (j in INS)
		sum (i in INS) (yij[i][j] + (tdf[i]+tex[i]+twb[i])*xij[i][j]) <= ti[j] + tdf[j] + tex[j];
	
	// xij=0 => yij=0
	forall (i in INS, j in INS)
		yij[i][j] <= U*xij[i][j];
	
	// xij=1 => yij=ti
	forall (i in INS, j in INS)
		ti[i] + U*xij[i][j] - U <= yij[i][j];
	forall (i in INS, j in INS)
		yij[i][j] <= ti[i];
	
	// Respect the data dependencies
	forall (i in INS, j in INS)
		(ti[i] + tdf[i] + tex[i] + twb[i])*E[i][j] <= ti[j]*E[i][j];
	
	// Makespan of the schedule
	forall (i in INS)
		z >= ti[i] + tdf[i]+tex[i]+twb[i];
}