package colorGraph;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * @author cristian.chilipirea
 * 
 */

public class Main {
	static int N = 10;
	static int COLORS = 3;
	static int[][] graph = { { 0, 1 }, { 0, 4 }, { 0, 5 }, { 1, 0 }, { 1, 2 }, { 1, 6 }, { 2, 1 }, { 2, 3 }, { 2, 7 },
			{ 3, 2 }, { 3, 4 }, { 3, 8 }, { 4, 0 }, { 4, 3 }, { 4, 9 }, { 5, 0 }, { 5, 7 }, { 5, 8 }, { 6, 1 },
			{ 6, 8 }, { 6, 9 }, { 7, 2 }, { 7, 5 }, { 7, 9 }, { 8, 3 }, { 8, 5 }, { 8, 6 }, { 9, 4 }, { 9, 6 },
			{ 9, 7 } };

	static void colorGraph(int[] colors, int step, ExecutorService tpn) {
		if (step == N) {
			printColors(colors);
			tpn.shutdown();
			return;
		}

		// for the node at position step try all possible colors
		for (int i = 0; i < COLORS; i++) {
			int[] newColors = colors.clone();
			newColors[step] = i;
			if (verifyColors(newColors, step)) {
				
				tpn.submit(new Runnable() {
					@Override
					public void run() {
						colorGraph(newColors, step + 1, tpn);
					}
				});
			}
		}
	}

	private static boolean verifyColors(int[] colors, int step) {
		for (int i = 0; i < step; i++) {
			if (colors[i] == colors[step] && isEdge(i, step))
				return false;
		}
		return true;
	}

	private static boolean isEdge(int a, int b) {
		for (int i = 0; i < graph.length; i++) {
			if (graph[i][0] == a && graph[i][1] == b)
				return true;
		}
		return false;
	}
	
	static void printColors(int[] colors) {
		String aux = "";
		for (int i = 0; i < colors.length; i++) {
			aux += colors[i] + " ";
		}
		System.out.println(aux);
	}

	public static void main(String[] args) {
		int[] colors = new int[N];
		ExecutorService tpn = Executors.newFixedThreadPool(4);
		tpn.submit(new Runnable() {
			
			@Override
			public void run() {
				colorGraph(colors, 0, tpn);
			}
		});
	}
}