package homework_1;
import java.util.Scanner;

public class problem4 {

	public static void main(String[] args) {
		
		Scanner sc = new Scanner(System.in);
		String input;
		
		System.out.print("Input data: ");
		input = sc.nextLine();
		
		System.out.print("Expected Output: ");
				
		for (int i=input.length()-1 ; i>=0 ; i--)
			System.out.print(input.substring(i,i+1));
		
        sc.close();
	}

}
