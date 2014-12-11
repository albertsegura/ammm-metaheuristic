/*********************************************
 * OPL 12.5 Model
 * Author: pedro
 * Creation Date: Dec 11, 2014 at 1:02:01 PM
 *********************************************/

main {
  	var inicio = 11;
	var n = 15;
	for (var i = inicio; i <= n; i++) {
		var ii = 1;
		if (i < 10)
			ii = "00"+i;
		else if (i < 100)
			ii = "0"+i;
		var name = "/home/pedro/workspace/Generator/Input"+ii+".dat";
		writeln("Input"+ii);
		var src = new IloOplModelSource("practica.mod");
		var def = new IloOplModelDefinition(src);
		var cplex = new IloCplex();
		var model = new IloOplModel(def,cplex);
		var data = new IloOplDataSource(name);
		model.addDataSource(data);
		model.generate();
		cplex.epgap=0.01;
		var start = cplex.getCplexTime();
		if (cplex.solve()) {
		  	var end = cplex.getCplexTime();
			writeln(cplex.getObjValue()+" "+start+" "+end);
		}
		else {
			cplex.getCplexTime();
			writeln("-1 "+cplex.getCplexStatus().toString());
		}
		model.end();
		data.end();
		def.end();
		cplex.end();
		src.end();
	}	
};