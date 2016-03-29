import java.io.*;
import java.util.ArrayList;

public class Kmeans{

	private final double TOLERANCE=.01;

	public static void main(String[] args){
		Kmeans km=new Kmeans();
		double[][] inst=km.read("proj02data.csv");
		int[] c=km.cluster(inst,4);
		for(int i=0; i<inst.length; i++)
			System.out.println(i+"\t"+c[i]);
	}

	public int[] cluster(double[][] inst, int k){
		int[] clusters=new int[inst.length];
		double[][] centroids=init(inst,k);
		double errThis=sse(inst,centroids,clusters), errLast=errThis+1;
		while(errLast-errThis>TOLERANCE){
			//reassign the clusters using assignClusters
      clusters = assignClusters(inst, centroids);
			//re-calculate the centroids
      centroids = recalcCentroids(inst, clusters, k);
			//re-calculate the error using sse
      errLast = errThis;
      errThis = sse(inst, centroids, clusters);
		}
		return clusters;
	}

	//finds initial clusters - no modifications necessary
	public double[][] init(double[][] inst, int k){
		int n=inst.length, d=inst[0].length;
		double[][] centroids=new double[k][d];
		double[][] extremes=new double[d][2];
		for(int i=0; i<d; i++)
			extremes[i][1]=Double.MAX_VALUE;
		for(int i=0; i<n; i++)
			for(int j=0; j<d; j++){
				extremes[j][0]=Math.max(extremes[j][0],inst[i][j]);
				extremes[j][1]=Math.min(extremes[j][1],inst[i][j]);
			}
		for(int i=0; i<k; i++)
			for(int j=0; j<d; j++)
				centroids[i][j]=Math.random()*(extremes[j][0]-extremes[j][1])+extremes[j][1];
		return centroids;
	}

	public int[] assignClusters(double[][] inst, double[][] centroids){
		int n=inst.length, d=inst[0].length, k=centroids.length;
		int[] rtn=new int[n];
    //TODO check if correct: rtn maps each inst to centroid?
		//for each instance
    for(int x = 0; x < inst.length; x++){
      double lowestDistance = Double.MAX_VALUE;
      double currentDistance;
      int chosenCluster = -1;
      for(int centroid = 0; centroid < k; centroid++){
        //calculate the distance to each of the different centroids
        currentDistance = euclid(inst[x], centroids[centroid]);
        if(currentDistance < lowestDistance){
          lowestDistance = currentDistance;
          chosenCluster = centroid;
        }
      }
      //and assign it to the cluster with the lowest distance
      rtn[x] = chosenCluster;
    }
		return rtn;
	}

	public double[][] recalcCentroids(double[][] inst, int[] clusters, int k){
		int n=inst.length, d=inst[0].length;
		double[][] centroids=new double[k][d];
		int[] cnt=new int[k];
    //TODO
		//use cnt to count the number of instances in each cluster
		//for each cluster
		for(int x = 0; x < clusters.length; x++){
			//for each attribute in this cluster
			for(int y = 0; y < )
				//add the value of the attribute from each instance in the cluster
		}

		//calculate the averages by dividing each attribute total by the count
		//do this for each centroid, each attribute
		//be careful not to divide by zero - if a cluster is emply, skip it
		return centroids;
	}

	public double sse(double[][] inst, double[][] centroids, int[] clusters){
		int n=inst.length, d=inst[0].length, k=centroids.length;
		double sum=0;
    //TODO test if correct: check if this makes any sense at all
		//iterate through all instances
    for(int x = 0; x < n; x++){
      //iterate through all centroids
      for(int z = 0; z < k; z++){
        //if an instance is in the current cluster, add the euclidean distance
				//between them to the sum
        if(clusters[x] == z){ //if the current instance is mapped to the current centroid
          sum += euclid(inst[x], centroids[z]);
        }
      }
    }
		return sum;
	}

	private double euclid(double[] inst1, double[] inst2){
		double sum=0;
    //TODO test if correct
    for(int x = 0; x < inst1.length; x++){
      sum += Math.pow(inst1[x] - inst2[x], 2);
    }
		//calculate the euclidean distance between inst1 and inst2
		return Math.sqrt(sum);
	}

	//prints out a matrix - can be used for debugging - no modifications necessary
	public void printMatrix(double[][] mat){
		for(int i=0; i<mat.length; i++){
			for(int j=0; j<mat[i].length; j++)
				System.out.print(mat[i][j]+"\t");
			System.out.println();
		}
	}

	//reads in the file - no modifications necessary
	public double[][] read(String filename){
		double[][] rtn=null;
		try{
			BufferedReader br=new BufferedReader(new FileReader(filename));
			ArrayList<String> lst=new ArrayList<String>();
			br.readLine();//skip first line of file - headers
			String line="";
			while((line=br.readLine())!=null)
				lst.add(line); //read file into lst
			int n=lst.size(), d=lst.get(0).split(",").length;
			rtn=new double[n][d]; //# rows and columns from lst
			for(int i=0; i<n; i++){
				String[] parts=lst.get(i).split(","); // get each object in column
				for(int j=0; j<d; j++)
					rtn[i][j]=Double.parseDouble(parts[j]); //put each object into return
			}
			br.close();
		}catch(IOException e){System.out.println(e.toString());}
		return rtn;
	}

}
