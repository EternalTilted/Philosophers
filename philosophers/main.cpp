#include <iostream>
#include <vector>
#include <thread>
#include <semaphore>
#include <windows.h>
#include <chrono>


//std::vector<HANDLE> fork{NULL, NULL, NULL, NULL, NULL};
HANDLE fork[5]{ NULL, NULL, NULL, NULL, NULL };
std::vector<bool> Stat{ true, true, true, true, true };


enum ConsoleColor
{
	Black = 0,
	Blue = 1,
	Green = 2,
	Cyan = 3,
	Red = 4,
	Magenta = 5,
	Brown = 6,
	LightGray = 7,
	DarkGray = 8,
	LightBlue = 9,
	LightGreen = 10,
	LightCyan = 11,
	LightRed = 12,
	LightMagenta = 13,
	Yellow = 14,
	White = 15
};

void SetColor(ConsoleColor text, ConsoleColor background)
{
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdOut, (WORD)((background << 4) | text));
}

class philosopher {
private:
	int TimeOfEat = 0;
	int TimeOfRest = 0;
	int status = 5;
	

	void Wait() {
		status = 0;
		while (1) {
			int num1 = rand() % 5;
			int num2 = rand() % 5;
			if (num1 == num2)continue;
			TimeOfEat = rand() % 3001 + 2000;
			if (Stat[num1] && Stat[num2]) {
				Stat[num1] = false;
				Stat[num2] = false;
				Eat(num1, num2);
			}
		}
	}

	void Eat(int num1, int num2) {
		HANDLE tmp[2]{ fork[num1], fork[num2]};

		WaitForMultipleObjects(2, tmp, true, INFINITE);
		
		status = 1;
		std::this_thread::sleep_for(std::chrono::milliseconds(TimeOfEat));
		TimeOfRest = rand() % 3001 + 2000;
		Stat[num1] = true;
		Stat[num2] = true;
		status = 2;
		ReleaseSemaphore(tmp[0], 1, NULL);
		ReleaseSemaphore(tmp[1], 1, NULL);

		Rest();
		
	}

	void Rest() {
		
		std::this_thread::sleep_for(std::chrono::milliseconds(TimeOfRest));
		Wait();
	}

public:

	philosopher() {
		
	};

	void start() {
		Wait();
	}

	int Status() {
		return status;
	};

};

void print(std::vector<philosopher*> ph) {
	while (1) {
		system("cls");
		for (int i = 0; i < 5; i++) {
			switch (ph[i]->Status())
			{
			case 0:
				std::cout << "Philosoph " << i + 1;
				
				std::cout<< " is waiting\n";
				break;
			case 1:
				std::cout << "Philosoph " << i + 1;
				SetColor(LightMagenta, Black);
				std::cout<< " is eating\n";
				SetColor(White, Black);
				break;
			case 2:
				std::cout << "Philosoph " << i + 1;
				SetColor(LightGreen, Black);
				std::cout << " is resting\n";
				SetColor(White, Black);
				break;
			default:
				std::cout << "Philosoph " << i + 1 << " somthing wrong\n";
				break;
			}
			//std::cout << ph[i]->Status() << "\n";
		}
		Sleep(100);
	}
}

int main() {

	
	fork[0] = CreateSemaphore(NULL, 1, 1, L"");
	fork[1] = CreateSemaphore(NULL, 1, 1, L"");
	fork[2] = CreateSemaphore(NULL, 1, 1, L"");
	fork[3] = CreateSemaphore(NULL, 1, 1, L"");
	fork[4] = CreateSemaphore(NULL, 1, 1, L"");

	
	std::vector<std::thread> ph;
	std::vector<philosopher*> ph1;

	for (int i = 0; i < 5;i++ ) {
		
		philosopher* p = new philosopher;

		ph1.push_back(p);

		ph.push_back(std::thread([&](int index) {
			ph1[index]->start();

			}, i));
	}
	
	std::thread t(print, ph1);

	for (int i = 0; i < 5; i++) {
		ph[i].join();
	}
}