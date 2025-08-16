package homework_1;
import java.util.Scanner;

public class problem3 {

	public static void main(String[] args) {
		
		int n;
		Scanner sc = new Scanner(System.in);
		n = sc.nextInt();
		
		for(int i=1;i<=9;i++) {
			System.out.println(n+" x "+i+" = "+(n*i));
		}
		
        sc.close();
	}

}
