import java.awt.*;
import java.awt.event.*;
import java.util.Stack;

public class calculator_20231508 extends Frame {
    private Label resultLabel;
    private String currentInput = "";
    private boolean resetInput = false;

    public calculator_20231508(String title) {
        super(title);
        setLayout(new BorderLayout());

        // 결과 출력 영역
        Panel resultPanel = new Panel();
        resultPanel.setLayout(new BorderLayout());
        resultPanel.setPreferredSize(new Dimension(400, 50));
        resultPanel.setBackground(Color.WHITE);

        resultLabel = new Label("0", Label.RIGHT);
        resultLabel.setFont(new Font("Arial", Font.BOLD, 24));
        resultPanel.add(resultLabel, BorderLayout.CENTER);
        add(resultPanel, BorderLayout.NORTH);

        // 버튼 영역
        Panel buttonPanel = new Panel();
        buttonPanel.setLayout(new FlowLayout(FlowLayout.RIGHT));

        String[][] buttonRows = {
            {"x!", "(", ")", "%", "AC"},
            {"sin", "ln", "7", "8", "9", "÷"},
            {"cos", "log", "4", "5", "6", "×"},
            {"tan", "√", "1", "2", "3", "-"},
            {"xʸ", "0", ".", "=", "+"}
        };

        for (String[] row : buttonRows) {
            Panel rowPanel = new Panel(new FlowLayout(FlowLayout.RIGHT));
            for (String text : row) {
                Button button = new Button(text);
                button.setFont(new Font("Arial", Font.BOLD, 16));
                button.setPreferredSize(new Dimension(130, 70));

                if (text.equals("=")) {
                    button.setForeground(Color.orange);
                } else if (!text.chars().allMatch(Character::isDigit) && !text.equals(".")) {
                    button.setForeground(Color.blue);
                }

                // 버튼 이벤트 처리
                button.addActionListener(e -> ButtonEvent(text));
                rowPanel.add(button);
            }
            buttonPanel.add(rowPanel);
        }
        add(buttonPanel, BorderLayout.CENTER);

        // 기본 창 설정
        setSize(1000, 510);
        setVisible(true);

        // 창 닫기 이벤트 처리
        addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosing(WindowEvent e) {
                System.exit(0);
            }
        });
    }

    // 버튼 이벤트 처리
    private void ButtonEvent(String text) {
        if (text.equals("AC")) {
            currentInput = "";
            resultLabel.setText("0");
            resetInput = false;
        } else if (text.equals("=")) {
            calculateResult();
        } else if ("+-×÷xʸ%()".contains(text)) {
            if (resetInput) {
                resetInput = false; //새로운 연산 시작
            }
            if (!currentInput.isEmpty()) {
                String[] tokens = currentInput.trim().split(" ");
                String lastToken = tokens[tokens.length - 1];
                
                
                //교체 처리
                if ("+-×÷".contains(lastToken)) {
                    if (text.equals("-")) {
                        if (lastToken.equals("-")) {
                            resultLabel.setText(currentInput.trim());
                        } else if (currentInput.endsWith(" ") || currentInput.endsWith("(")) {
                            currentInput += text;
                        }
                    } else if ("+-×÷".contains(text) && !lastToken.equals("-")) {
                        currentInput = currentInput.substring(0, currentInput.length() - 2) + text + " ";
                    }
                } else if (lastToken.equals("-") && currentInput.matches(".*[+-×÷] -$")) {
                    resultLabel.setText(currentInput.trim()); 
                } else if (lastToken.matches("-?\\d+(\\.\\d+)?") || ")".equals(lastToken)) {
                    currentInput += " " + text + " ";
                } else {
                    if (text.equals("-")) {
                        currentInput += text; 
                    }
                }
                
                
            } else {
                // 빈 입력에서 음수로 시작
                if (text.equals("-")) {
                    currentInput += text;
                    
                }
            }
            resultLabel.setText(currentInput.trim());
        } else if ("x! log ln √ sin cos tan".contains(text)) {
            if (resetInput) {
                resetInput = false;
            }
            currentInput += " " + text + " ";
            resultLabel.setText(currentInput.trim());
        } else if (text.equals(".")) {
            if (currentInput.isEmpty() || resetInput) {
                currentInput = "0.";
                resetInput = false;
            } else if (!currentInput.endsWith(".")) {
                currentInput += ".";
            }
            resultLabel.setText(currentInput);
        } else if (text.matches("\\d")) { 
            if (resetInput) {
                currentInput = ""; 
                resetInput = false;
            }
            currentInput += text;
            resultLabel.setText(currentInput);
        }
    }
    
    

    // 계산 수행
    private void calculateResult() {
        try {
            String postfix = toPostfix(currentInput.trim());
            double result = evaluatePostfix(postfix);
            result = Math.round(result * 1e8) / 1e8; 
            resultLabel.setText(String.valueOf(result));
            currentInput = String.valueOf(result);
            resetInput = true;
        } catch (ArithmeticException e) {
            resultLabel.setText("Error: " + e.getMessage());
            currentInput = "";
        } catch (Exception e) {
            resultLabel.setText("Error");
            currentInput = "";
        }
    }

    // 중위 표기법을 후위 표기법으로 변환
    private String toPostfix(String expression) {
        Stack<String> stack = new Stack<>();
        StringBuilder output = new StringBuilder();
    
        String[] tokens = expression.split(" ");
        for (int i = 0; i < tokens.length; i++) {
            String token = tokens[i];
            if (token.matches("-?\\d+(\\.\\d+)?")) { 
                output.append(token).append(" ");
            } else if ("(".equals(token)) { 
                stack.push(token);
            } else if (")".equals(token)) {
                while (!stack.isEmpty() && !"(".equals(stack.peek())) {
                    output.append(stack.pop()).append(" ");
                }
                if (!stack.isEmpty() && "(".equals(stack.peek())) {
                    stack.pop(); 
                }
            } else { 
                while (!stack.isEmpty() && precedence(stack.peek()) >= precedence(token)) {
                    output.append(stack.pop()).append(" ");
                }
                stack.push(token);
            }
        }
    
        while (!stack.isEmpty()) {
            output.append(stack.pop()).append(" ");
        }
    
        return output.toString().trim();
    }
    
    

    // 후위 표기법 평가
    private double evaluatePostfix(String postfix) {
        Stack<Double> stack = new Stack<>();
        String[] tokens = postfix.split(" ");
        for (String token : tokens) {
            if (token.matches("-?\\d+(\\.\\d+)?")) { 
                stack.push(Double.parseDouble(token));
            } else if ("x!".equals(token)) {
                double a = stack.pop();
                stack.push(factorial((int) a));
            } else if ("log".equals(token)) {
                stack.push(Math.log10(stack.pop()));
            } else if ("ln".equals(token)) {
                stack.push(Math.log(stack.pop()));
            } else if ("√".equals(token)) {
                stack.push(Math.sqrt(stack.pop()));
            } else if ("sin".equals(token)) {
                stack.push(Math.sin(Math.toRadians(stack.pop())));
            } else if ("cos".equals(token)) {
                stack.push(Math.cos(Math.toRadians(stack.pop())));
            } else if ("tan".equals(token)) {
                stack.push(Math.tan(Math.toRadians(stack.pop())));
            } else if ("%".equals(token)) {
                double a = stack.pop();
                stack.push(a * 0.01); 
            } else { 
                double b = stack.pop();
                double a = stack.pop();
                if ("÷".equals(token) && b == 0) { 
                    throw new ArithmeticException("Division by zero");
                }
                stack.push(applyOperator(a, b, token));
            }
        }
        return stack.pop();
    }
    

    private int precedence(String operator) {
        switch (operator) {
            case "+":
            case "-":
                return 1;
            case "×":
            case "÷":
                return 2;
            case "xʸ":
                return 3;
            default:
                return 0;
        }
    }

    private double applyOperator(double a, double b, String operator) {
        switch (operator) {
            case "+": return a + b;
            case "-": return a - b;
            case "×": return a * b;
            case "÷": return b != 0 ? a / b : Double.NaN;
            case "%": return a * 0.01; 
            case "xʸ": return Math.pow(a, b);
            default: return 0;
        }
    }

    private double factorial(int n) {
        if (n <= 1) return 1;
        return n * factorial(n - 1);
    }

    public static void main(String[] args) {
        new calculator_20231508("Yuyeong's Calculator");
    }
}
