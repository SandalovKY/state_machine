#include <iostream>
#include <string>
#include <vector>
#include <bitset>

void transf()
{
	const int inputs = 4;
	const int states = 14;

	char in_w[4];
	int trans[inputs][states];
	
	std::cout << "Введите матрицу переходов:\n";
	for (int i = 0; i < inputs; i++)
	{
		for (int j = 0; j < states; j++)
		{
			std::cin >> in_w;
			if (in_w[0] == '-')
			{
				trans[i][j] = 0;
			}
			else
			{
				trans[i][j] = std::stoi(in_w) - 1;
			}
		}
	}

	std::cout << "Закодированная матрица переходов:\n";
	for (int i = 0; i < inputs; i++)
	{
		for (int j = 0; j < states; j++)
		{
			std::cout << std::bitset<4>(trans[i][j]) << '\t';
		}
		std::cout << std::endl;
	}

	int d0{ 1 };
	int d1{ 2 };
	int d2{ 4 };
	int d3{ 8 };

	std::vector<int> d0_ar{};
	std::vector<int> d1_ar{};
	std::vector<int> d2_ar{};
	std::vector<int> d3_ar{};

	for (int i = 0; i < inputs; i++)
	{
		for (int j = 0; j < states; j++)
		{
			if (trans[i][j] & d0)
			{
				d0_ar.push_back(i*16+j);
			}
			if (trans[i][j] & d1)
			{
				d1_ar.push_back(i*16+j);
			}
			if (trans[i][j] & d2)
			{
				d2_ar.push_back(i*16+j);
			}
			if (trans[i][j] & d3)
			{
				d3_ar.push_back(i*16+j);
			}
		}
	}

	std::string str0(64, '0');
	for (auto& el: d0_ar)
	{
		str0[el] = '1';
	}
	std::string str1(64, '0');
	for (auto& el: d1_ar)
	{
		str1[el] = '1';
	}
	std::string str2(64, '0');
	for (auto& el: d2_ar)
	{
		str2[el] = '1';
	}
	std::string str3(64, '0');
	for (auto& el: d3_ar)
	{
		str3[el] = '1';
	}
	std::cout << "Полученные СДНФ:\n";
	std::cout << str0 << std::endl << str1 << std::endl <<  str2 << std::endl << str3  << std::endl << std::endl; 
}

void outputs()
{
	const int inputs = 4;
	const int states = 14;

	char in_w[4];
	int outputs[inputs][states];
	
	std::cout << "Введите матрицу выходов:\n";
	for (int i = 0; i < inputs; i++)
	{
		for (int j = 0; j < states; j++)
		{
			std::cin >> in_w;
			if (in_w[0] == '-')
			{
				outputs[i][j] = 0;
			}
			else
			{
				outputs[i][j] = std::stoi(in_w);
			}
		}
	}

	std::cout << "Закодированная матрица выходов:\n";
	for (int i = 0; i < inputs; i++)
	{
		for (int j = 0; j < states; j++)
		{
			std::cout << std::bitset<2>(outputs[i][j]) << '\t';
		}
		std::cout << std::endl;
	}

	int out_1{ 1 };
	int out_2{ 2 };

	std::vector<int> out1_ar{};
	std::vector<int> out2_ar{};

	for (int i = 0; i < inputs; i++)
	{
		for (int j = 0; j < states; j++)
		{
			if (outputs[i][j] & out_1)
			{
				out1_ar.push_back(i*16+j);
			}
			if (outputs[i][j] & out_2)
			{
				out2_ar.push_back(i*16+j);
			}
		}
	}
	std::string str1(64, '0');
	for (auto& el: out1_ar)
	{
		str1[el] = '1';
	}
	std::string str2(64, '0');
	for (auto& el: out2_ar)
	{
		str2[el] = '1';
	}
	std::cout << "Полученные СДНФ:\n";
	std::cout << str1 << std::endl <<  str2 << std::endl << std::endl; 
}

int main()
{
	transf();
	outputs();
}

