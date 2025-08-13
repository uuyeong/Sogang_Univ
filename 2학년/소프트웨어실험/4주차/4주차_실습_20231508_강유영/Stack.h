#include "LinkedList.h"


//1. 템플릿 클래스로 확장해야함
//2. Stack형식으로 Delete 함수 재정의해야함
//주의: first, current_size는 class의 멤버 변수이기 때문에 this 포인터를 사용하여 가져와야함

//LinkedList class를 상속받음
template <typename T>
class Stack : public LinkedList<T>{
	public:
		bool Delete (int &element){
			//first가 0이면 false반환
            if(this->first == 0) return false;
            
			// LinkedList와 달리 Stack은 current가 가리키는 곳을 삭제
            element = this->first->data; //element 첫 번째 노드로 초기화
            
            Node<T> *f = this->first;
            this->first = this->first->link; //현재 노드를 삭제하기 위해 다음노드를 새로운 첫 번째 노드로 설정한다
            delete f; //복사해둔 첫 번째 노드를 삭제
            this->current_size--; //연결리스트 크기 줄이기
            
			return true;
			}
};

