package homework_1;

public class problem2 {

	public static void main(String[] args) {
		int out=0, in=15;
		
		for(int i=0;i<8;i++) {
			for(int j=0;j<out;j++) System.out.print(" ");
			System.out.print("*");
			for(int j=0;j<in;j++) System.out.print(" ");
			System.out.print("*");
			for(int j=0;j<out;j++) System.out.print(" ");
			in -=2;
			out++;	
			System.out.println();
		}
		
		for(int j=0;j<out;j++) System.out.print(" ");
		System.out.print("*");
		System.out.println();

		
		for(int i=0;i<8;i++) {
			in +=2;
			out--;	
			for(int j=0;j<out;j++) System.out.print(" ");
			System.out.print("*");
			for(int j=0;j<in;j++) System.out.print(" ");
			System.out.print("*");
			for(int j=0;j<out;j++) System.out.print(" ");
			
			System.out.println();
		}
		
		
		
	}

}
