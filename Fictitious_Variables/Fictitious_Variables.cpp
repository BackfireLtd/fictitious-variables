#include "stdafx.h" 
#include <iostream> 
#include <string> 
#include <vector> 
#include <Windows.h>

using namespace std;

const bool FORWARD = true;
const bool BACKWARD = false;
const string WRONG = "Неверный ввод!";
const int DELAY = 200;

bool yes_or_no(string message)
{
	char reply;

	bool firstTry = true;
	do
	{
		if (!firstTry)
		{
			system("color 0C");
			while (cin.peek() != '\n')
				cin.get();
			cout << WRONG << endl;
			Sleep(DELAY);
		}

		cout << message;
		cin >> reply;

		firstTry = false;
	} while (cin.peek() != '\n' || (reply != 'н' && reply != 'Н' && reply != 'д' && reply != 'Д'));

	if (reply == 'д' || reply == 'Д')
		return true;

	return false;
}

bool operation(bool l_operand, bool r_operand, char oper)
{
	if (oper == '&')
		l_operand = l_operand && r_operand;
	else if (oper == '|')
		l_operand = l_operand || r_operand;
	else if (oper == '+')
		l_operand = (!l_operand && r_operand) || (l_operand && !r_operand);
	else if (oper == '>')
		l_operand = !l_operand || r_operand;
	else if (oper == '=')
		l_operand = l_operand == r_operand;

	return l_operand;
}

string inner_exp(string exp, int start, bool direction)
{
	string in_exp;
	int brackets = 1;
	int i = start;
	while (brackets > 0)
	{
		if (exp[i] == '(')
			direction ? brackets++ : brackets--;
		else if (exp[i] == ')')
			direction ? brackets-- : brackets++;

		if (brackets > 0)
			in_exp += exp[i];

		direction ? i++ : i--;
	}

	return in_exp;
}

bool expression(string exp, int x, int y, int z)
{
	bool l_operand = false, r_operand = false, after = false;
	char oper;

	for (int i = 0; i < exp.length(); i++)
	{
		switch (exp[i])
		{
			case 'x':
				after ? r_operand = x : l_operand = x;
				if (after)
				{
					l_operand = operation(l_operand, r_operand, oper);
					after = false;
				}
				break;
			case 'y':
				after ? r_operand = y : l_operand = y;
				if (after)
				{
					l_operand = operation(l_operand, r_operand, oper);
					after = false;
				}
				break;
			case 'z':
				after ? r_operand = z : l_operand = z;
				if (after)
				{
					l_operand = operation(l_operand, r_operand, oper);
					after = false;
				}
				break;
			case '!':
				i++;
				if (exp[i] == 'x')
					after ? r_operand = !x : l_operand = !x;
				else if (exp[i] == 'y')
					after ? r_operand = !y : l_operand = !y;
				else if (exp[i] == 'z')
					after ? r_operand = !z : l_operand = !z;
				else if (exp[i] == '(')
				{
					string in_exp = inner_exp(exp, i + 1, FORWARD);
					after ? r_operand = !expression(in_exp, x, y, z) : l_operand = !expression(in_exp, x, y, z);
					i += in_exp.length() + 1;
				}

				if (after)
				{
					l_operand = operation(l_operand, r_operand, oper);
					after = false;
				}
				break;
			case '&':
				after = true;
				oper = '&';
				break;
			case '|':
				after = true;
				oper = '|';
				break;
			case '+':
				after = true;
				oper = '+';
				break;
			case '>':
				after = true;
				oper = '>';
				break;
			case '=':
				after = true;
				oper = '=';
				break;
			case '(':
				string in_exp = inner_exp(exp, i + 1, FORWARD);
				after ? r_operand = expression(in_exp, x, y, z) : l_operand = expression(in_exp, x, y, z);
				if(after)
				{
					l_operand = operation(l_operand, r_operand, oper);
					after = false;
				}
				i += in_exp.length() + 1;
				break;
		}
	}

	return l_operand;
}

string include_omitted_ands(string exp)
{
	string edited = exp;
	int i = 0;
	bool need_operator = false;

	while (i < edited.length())
	{
		if (edited[i] == '!' || edited[i] == 'x' || edited[i] == 'y' || edited[i] == 'z' || edited[i] == '(')
		{
			if (need_operator)
			{
				edited.insert(i, "&");
				need_operator = false;
			}
			else
				need_operator = true;

			if (edited[i] == '!')
				i++;

			if (edited[i] == '(')
			{
				string in_exp = inner_exp(edited, i + 1, FORWARD);
				string included = include_omitted_ands(in_exp);
				edited.erase(i + 1, in_exp.length());
				edited.insert(i + 1, included);
				i += included.length();
			}
		}
		else if (edited[i] == '&' || edited[i] == '|' || edited[i] == '+' || edited[i] == '>' || edited[i] == '=')
			need_operator = false;

		i++;
	}

	return edited;
}

string set_priority(string exp, char oper)
{
	string edited = exp;
	int i = 1;

	while (i < edited.length())
	{
		if (edited[i] == oper)
		{
			int j = i - 1;
			while (edited[j] == ' ')
				j--;

			if (edited[j] == 'x' || edited[j] == 'y' || edited[j] == 'z')
			{
				if (j > 0 && edited[j - 1] == '!')
					edited.insert(j - 1, "(");
				else
					edited.insert(j, "(");
			}
			else if (edited[j] == ')')
			{
				int exp_start = j - inner_exp(edited, j - 1, BACKWARD).length() - 1;

				if (exp_start > 0 && edited[exp_start - 1] == '!')
					edited.insert(exp_start - 1, "(");
				else
					edited.insert(exp_start, "(");
			}

			j = i + 2;
			while (edited[j] == ' ' || edited[j] == '!')
				j++;

			if (edited[j] == 'x' || edited[j] == 'y' || edited[j] == 'z')
				edited.insert(j + 1, ")");
			else if (edited[j] == '(')
			{
				int exp_end = j + inner_exp(edited, j + 1, FORWARD).length() + 1;

				edited.insert(exp_end + 1, ")");
			}

			i++;
		}

		i++;
	}

	return edited;
}

string clear_brackets(string exp)
{
	string edited = exp;

	int i = 0;
	while (i < edited.length())
	{
		if (edited[i] == '(' && edited[i + 1] == '(' && inner_exp(edited, i + 1, FORWARD).length() -
			inner_exp(edited, i + 2, FORWARD).length() == 2)
		{
			edited.erase(edited.begin() + i + 1 + inner_exp(edited, i + 1, FORWARD).length());
			edited.erase(edited.begin() + i);
			i--;
		}

		i++;
	}

	if(edited[0] == '(' && edited.length() - inner_exp(edited, 1, FORWARD).length() == 2)
	{
		edited.erase(edited.begin() + edited.length() - 1);
		edited.erase(edited.begin());
	}

	return edited;
}

void show_table(vector<int> f)
{
	cout << "Таблица истинности:" << endl;
	Sleep(DELAY);
	cout << "x y z f" << endl;
	for (int i = 0; i < 8; i++)
	{
		Sleep(DELAY);
		int n = i;

		cout << n / 4 << ' ';
		if (n >= 4)
			n -= 4;
		cout << n / 2 << ' ';
		if (n >= 2)
			n -= 2;
		cout << n << ' ' << f[i] << endl;
	}
}

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	do
	{
		system("color 0F");

		char reply;
		bool firstTry = true;
		do
		{
			if (!firstTry)
			{
				system("color 0C");
				while (cin.peek() != '\n')
					cin.get();
				cout << WRONG << endl;
				Sleep(DELAY);
			}

			cout << "Задать функцию вектором значений или логическим выражением? (1/2): ";
			cin >> reply;

			firstTry = false;
		} while (cin.peek() != '\n' || (reply != '1' && reply != '2'));

		vector<int> f(8);

		if (reply == '1')
		{
			system("color 0A");

			cout << "Введите f(x, y, z) = ";

			string values;
			cin >> values;
			for (int i = 0; i < 8; i++)
				f[i] = atoi(values.substr(i, 1).c_str());

			show_table(f);
		}
		else if (reply == '2')
		{
			system("color 0B");

			cout << "Обозначения операций:" << endl;
			Sleep(DELAY);
			cout << "& - конъюнкция (можно опустить)" << endl;
			Sleep(DELAY);
			cout << "| - дизъюнкция" << endl;
			Sleep(DELAY);
			cout << "! - отрицание" << endl;
			Sleep(DELAY);
			cout << "+ - сложение по модулю 2" << endl;
			Sleep(DELAY);
			cout << "> - импликация" << endl;
			Sleep(DELAY);
			cout << "= - эквивалентность" << endl;
			Sleep(DELAY);
			cout << "( ) - приоритет выражения" << endl;
			Sleep(DELAY);

			cout << "Введите f(x, y, z) = ";

			string exp;
			cin.get();
			getline(cin, exp);

			exp = include_omitted_ands(exp);
			exp = set_priority(set_priority(exp, '&'), '|');
			exp = clear_brackets(exp);
			cout << "Преобразованное выражение: " << exp << endl;

			for (int i = 0; i < 8; i++)
			{
				int n = i;

				int x = n / 4;
				if (n >= 4)
					n -= 4;
				int y = n / 2;
				if (n >= 2)
					n -= 2;
				int z = n;

				f[i] = expression(exp, x, y, z);
			}

			Sleep(DELAY);
			show_table(f);
		}

		vector<char> sign = { 'x', 'y', 'z' };
		vector<char> fict;

		if (f[0] == f[4] && f[1] == f[5] && f[2] == f[6] && f[3] == f[7])
		{
			sign.erase(find(sign.begin(), sign.end(), 'x'));
			fict.push_back('x');
		}

		if (f[0] == f[2] && f[1] == f[3] && f[4] == f[6] && f[5] == f[7])
		{
			sign.erase(find(sign.begin(), sign.end(), 'y'));
			fict.push_back('y');
		}

		if (f[0] == f[1] && f[2] == f[3] && f[4] == f[5] && f[6] == f[7])
		{
			sign.erase(find(sign.begin(), sign.end(), 'z'));
			fict.push_back('z');
		}

		if (sign.size() > 0)
		{
			Sleep(DELAY);
			cout << "Существенные: ";
			for (int i = 0; i < sign.size(); i++)
				i == sign.size() - 1 ? cout << sign[i] << endl : cout << sign[i] << ", ";
		}

		if (fict.size() > 0)
		{
			Sleep(DELAY);
			cout << "Фиктивные: ";
			for (int i = 0; i < fict.size(); i++)
				i == fict.size() - 1 ? cout << fict[i] << endl : cout << fict[i] << ", ";

			if (sign.size() > 0)
			{
				Sleep(DELAY);
				cout << "\"Очищенная\" таблица истинности:" << endl;

				Sleep(DELAY);
				vector<int> f_sign;
				if (sign.size() == 1)
				{
					if (sign[0] == 'x')
					{
						f_sign = { 0, 3 };
						cout << "x f" << endl;
					}
					else if (sign[0] == 'y')
					{
						f_sign = { 0, 2 };
						cout << "y f" << endl;
					}
					else if (sign[0] == 'z')
					{
						f_sign = { 0, 1 };
						cout << "z f" << endl;
					}

				}
				else if (sign.size() == 2)
				{
					if (fict[0] == 'x')
					{
						f_sign = { 0, 1, 2, 3 };
						cout << "y z f" << endl;
					}
					if (fict[0] == 'y')
					{
						f_sign = { 0, 1, 4, 5 };
						cout << "x z f" << endl;
					}
					if (fict[0] == 'z')
					{
						f_sign = { 0, 2, 4, 6 };
						cout << "x y f" << endl;
					}
				}

				for (int i = 0; i < f_sign.size(); i++)
				{
					Sleep(DELAY);

					if (f_sign.size() == 2)
						cout << i << ' ' << f[f_sign[i]] << endl;
					else if (f_sign.size() == 4)
					{
						int n = i;
						cout << i / 2 << ' ';
						if (n >= 2)
							n -= 2;
						cout << n << ' ' << f[f_sign[i]] << endl;
					}
				}
			}
		}

		Sleep(DELAY);
	} while (yes_or_no("Запустить программу заново? (Д/Н): "));

	return 0;
}

