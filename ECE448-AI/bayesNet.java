import java.io.*;
import java.io.PrintWriter;
import java.util.Random;
import java.util.Scanner;

public class bayesNet {

/* private static double sample(bayeNode[] bnet, int samples, int[] goal, int[] conds);
Inputs: bnet - array of bayeNodes, samples - integer number of samples desired, 
goal - array of goal conditions, conds - array of reject conditions
Outputs: The desired probability calculated from the samples
*/
  private static double sample(bayeNode[] bnet, int samples, int[] goal, int[] conds) {

  	double total = 0, count = 0;

  	int cptInd;

  	boolean correct, reject, unrestricted;

  	if (conds.length == 1) unrestricted = true;
  	else unrestricted = false;

  	for (int i=0;i<samples;i++) {
  		reject = false;


  		for (int j=0;j<bnet.length;j++) {

  			cptInd = 0;

  			for (int k=0;k<bnet[j].numPar;k++) {
  				if (!bnet[bnet[j].parents[k]].value)
  				cptInd += Math.pow(2,bnet[j].numPar-k-1);
  			}

  			if (Math.random() < bnet[j].cpt[cptInd]) bnet[j].value = true;
  			else bnet[j].value = false;

  		} 

  		if (!unrestricted) {
  			for (int j=0;j<conds.length;j+=2) {

  				if ((bnet[conds[j]].value)!=(conds[j+1]!=0)) {
  					reject = true;
  					break;
  				}

  			}
  		}

  		if (!reject) {
  			total++;

  			correct = true;
  			for (int j=0;j<goal.length;j+=2) {

  				if ((bnet[goal[j]].value)!=(goal[j+1]!=0)) correct = false;
  				
  			}

  			if (correct) count++;

  		}

  	}

  	return (count/total);

  }

/* run program with the following argument:
arg0: filename of txt describing nextwork
arg1: number of samples desired
arg2: number of goal condition pairs
arg3-arg(2+arg2*2): indicators of goals to look for,
in the format of "index value" - so if you 
want to look for all non-rejected samples in which node 5
has the value "true", add "5 1" to the args
arg(3+arg2*2) and onwards: same as above, but listing
conditions for non-rejection     */

/* Example usage: for the calculations required on the hw, the following commands should be used:
4.a) java bayesNet network1.txt <numSamples> 7 0 1 1 0 2 1 3 0 4 1 5 1 6 0
4.b) java bayesNet network1.txt <numSamples> 1 4 1 6 0 
4.c) java bayesNet network1.txt <numSamples> 1 5 1 0 0 1 1
5.   java bayesNet network2.txt <numSamples> 1 2 1 0 1 3 0 */


  public static void main(String[] args) {

// declarations
  	String fileName = args[0];
  	String line;
  	int i,j;
  	bayeNode[] network = null;

	int	numSamples = Integer.parseInt(args[1]);
  	int numGoals = Integer.parseInt(args[2]);

  	int[] goals = new int[numGoals*2];

// parsing goal arguments
  	for (i=0;i<numGoals*2;i++) {

  		goals[i] = Integer.parseInt(args[i+3]);

  	}

  	int numConds;
  	if (args.length - (numGoals*2+3) > 0) numConds = (args.length - numGoals*2 - 3);
  	else numConds = 1;

  	int[] conditions = new int[numConds];

// parsing conditions arguments (if any)
  	if (args.length>numGoals*2+3) {

  	for (i=0;i<numConds;i++) {

  		conditions[i] = Integer.parseInt(args[i+numGoals*2+3]);

  	}

  	} else {

// dummy conditions parameter if none given
  	conditions[0] = -1;

  	}	

// reading in the network text file
  	try {

  	Scanner ls = new Scanner(new FileReader(fileName));

	int numVars = ls.nextInt();

	network = new bayeNode[numVars];

	int index;

// each piece of data placed appropriately into its spot
	for (i=0;i<numVars;i++) {

		index = ls.nextInt();
		 
		network[index] = new bayeNode();
		network[index].numPar = ls.nextInt();
		network[index].parents = new int[network[index].numPar];
		network[index].cpt = new double[(int)Math.pow(2,network[index].numPar)];

		for (j=0;j<network[index].numPar;j++) {

			network[index].parents[j] = ls.nextInt();

		}

	}

// iterating through the last lines to fill the CPTs
	for (i=0;i<numVars;i++) {

		index = ls.nextInt();

		for (j=0;j<(int)Math.pow(2,network[index].numPar);j++) {

			network[index].cpt[j] = ls.nextDouble();

		}

	}

	ls.close();

}

	catch(FileNotFoundException ex) {
            System.out.println("Unable to open file specified");                
    }

    catch(IOException ex) {
            System.out.println("Error reading file");                  
    }

// calling and printing output from the rejction sampling function
double output = sample(network, numSamples, goals, conditions);
System.out.println(output);
// System.out.println("The probability calculated for the input goals and conditions given over " + numSamples + " samples is: " + output);

}

}

// class representing a single node of the graph
class bayeNode {

		public boolean value;
        public int numPar;
        public int[] parents;
        public double[] cpt;

}

