package homework_1;

public class problem1 {

	public static void main(String[] args) {
		int a, b;
		
		do {
			a = (int)(7*Math.random());
			b = (int)(7*Math.random());
			
			System.out.print("("+a+","+b+") ");
			
			
		} while((a+b) != 7);
			

	}

}
