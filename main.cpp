#include "stdafx.h"
#include <ctime>
#include <string>
#include <iostream>
#include <conio.h>
#include <thread>
#include <mutex>
#include <fstream>
using std::ifstream;
#define CURL_STATICLIB
#include <curl.h>
using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::thread;
using std::mutex;

void answers_search(string, string, int&, mutex&);
void kmp_search(string, string, int&);
int kmp_search1(string, string);

size_t WriteCallback(void *ptr, size_t size, size_t nmemb, string*userdata)
{
	userdata->append((char *)ptr, size * nmemb);
	return size * nmemb;
}

//---------------------------------------------------------------------------------------------------

int main()
{
	SetConsoleOutputCP(CP_UTF8); setvbuf(stdout, nullptr, _IOFBF, 2000); // Волшебная палочка. Позволяет нормально выводить UTF-8 в консоль

	CURL *curl = curl_easy_init();
	CURLcode res;

	if (curl)
	{
		string pattern = "question\"", url;
		bool ch;
		double start;
		char in;

		while (1)
		{
			cout << "Press \'R\' to perform: " << std::flush;
			if ((in = _getche()) == 'r')
			{
				string responseString;
				start = clock();
				cout << endl;
				ch = false;

				ifstream fin;
				string buf;
				fin.open("input.txt");
				while (!fin.eof())
				{
					fin >> buf;
					responseString += buf;
					responseString += ' ';
				}
				fin.close();

				/*curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, FALSE);
				curl_easy_setopt(curl, CURLOPT_URL, "https://api.vk.com/method/execute.getStartData?build_ver=3067&need_leaderboard=1&func_v=2&access_token=761a01f3e9bf859a2755cfe1e6f8e42d7d8f4519b994d7348969860a21d994ced16012be9d3b8b1e462dc&v=5.73&lang=ru&https=1");
				curl_easy_setopt(curl, CURLOPT_PROXY, "http://192.168.1.100:8888/");
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);
				res = curl_easy_perform(curl);
				cout << "Request to api.vk/com: " << res << endl;*/

				if (1)//(!(res != 0))
				{
					string question, answ1, answ2, answ3;
					int score1 = 0, score2 = 0, score3 = 0;
					int i = 650;//553;

					//функция поиска вопроса
					{
						for (; !('\"' == responseString[i] && responseString[i + 1] == 'p' && 'h' == responseString[i + 2]); ++i)
						{
							if (pattern[0] != responseString[i]) {}
							else
							{
								++i;
								for (short j = 1; j < pattern.length(); ++i, ++j)
								{
									if (pattern[j] == responseString[i] && pattern[j] == '\"')
									{
										i += 11;
										for (; responseString[i] != '\"'; i++) if (!(responseString[i] == 'В' && responseString[i+1] == '«') && !(responseString[i] == 'В' && responseString[i + 1] == '»')) { question += responseString[i]; };
										ch = true;
										break;
									}
									else if (pattern[j] != responseString[i]) { ++i; break; }
								}
								if (ch)	break;
							}
						}

						pattern = "text\"";
						i += 38;

						for (; !('\"' == responseString[i] && responseString[i + 1] == 'p' && 'h' == responseString[i + 2]); ++i)
						{
							if (pattern[0] != responseString[i]) {}
							else
							{
								++i;
								for (short j = 1; j < pattern.length(); ++i, ++j)
								{
									if (pattern[j] == responseString[i] && pattern[j] == '\"')
									{
										i += 3;
										for (; responseString[i] != '\"'; i++)
											if (!(responseString[i] == 'В' && responseString[i + 1] == '«') && !(responseString[i] == 'В' && responseString[i + 1] == '»')) { answ1 += responseString[i]; };
										ch = true;
										break;
									}
									else if (pattern[j] != responseString[i]) { ++i; break; }
								}
								if (ch)	break;
							}
						}

						i += 28;

						for (; !('\"' == responseString[i] && responseString[i + 1] == 'p' && 'h' == responseString[i + 2]); ++i)
						{
							if (pattern[0] != responseString[i]) {}
							else
							{
								++i;
								for (short j = 1; j < pattern.length(); ++i, ++j)
								{
									if (pattern[j] == responseString[i] && pattern[j] == '\"')
									{
										i += 3;
										for (; responseString[i] != '\"'; i++) if (!(responseString[i] == 'В' && responseString[i + 1] == '«') && !(responseString[i] == 'В' && responseString[i + 1] == '»')) { answ2 += responseString[i]; };
										ch = true;
										break;
									}
									else if (pattern[j] != responseString[i]) { ++i; break; }
								}
								if (ch)	break;
							}
						}

						i += 28;

						for (; !('\"' == responseString[i] && responseString[i + 1] == 'p' && 'h' == responseString[i + 2]); ++i)
						{
							if (pattern[0] != responseString[i]) {}
							else
							{
								++i;
								for (short j = 1; j < pattern.length(); ++i, ++j)
								{
									if (pattern[j] == responseString[i] && pattern[j] == '\"')
									{
										i += 3;
										for (; responseString[i] != '\"'; i++) if (!(responseString[i] == 'В' && responseString[i + 1] == '«') && !(responseString[i] == 'В' && responseString[i + 1] == '»')) { answ3 += responseString[i]; };
										ch = true;
										break;
									}
									else if (pattern[j] != responseString[i]) { ++i; break; }
								}
								if (ch)	break;
							}
						}
					}

					//проверка, есть ли вопрос 
					//проверка изменился ли вопрос
					if (!ch) std::cout << "String not found!" << std::endl << "End point: " << responseString[i] << responseString[i + 1] << responseString[i + 2] << responseString[i + 3] << std::endl;
					
					else
					{
						{
							cout << "Question: " << question << endl;
							char *encoded = curl_easy_escape(curl, question.c_str(), 0);
							if (encoded)
							{
								question = string(encoded);
								curl_free(encoded);
							}
							url = "https://www.googleapis.com/customsearch/v1?key=AIzaSyDx8XJCmJGO3AlHNjZTjCqL5MZCxMzZc1c&cx=014014728960136400655:zkid_r-hvto&q=" + question;

							responseString = "";
							curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, FALSE);
							curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // c_str() Формирует массив строк в стиле си. И возвращает указатель на него.
							curl_easy_setopt(curl, CURLOPT_PROXY, "http://192.168.1.100:8888/");
							curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
							curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);
							res = curl_easy_perform(curl);
							cout << "Request to google api: " << res << endl;
						}

						mutex mtx;
						thread t1(answers_search, responseString, answ1, std::ref(score1), std::ref(mtx));
						thread t2(answers_search, responseString, answ2, std::ref(score2), std::ref(mtx));
						thread t3(answers_search, responseString, answ3, std::ref(score3), std::ref(mtx));
						t1.join();
						t2.join();
						t3.join();

						string response2, template1 = "\"link\"";
						thread t[15];
						int t_number = 1;

						for (size_t i = -1; i != 0; i++)
						{
							i = kmp_search1(responseString, template1);

							if (i)
							{
								i += 3;
								url = "";
								while (responseString[i] != '\"')
								{
									url += responseString[i];
									++i;
								}
								curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, FALSE);
								curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // c_str() Формирует массив строк в стиле си. И возвращает указатель на него.
								curl_easy_setopt(curl, CURLOPT_PROXY, "http://192.168.1.100:8888/");
								curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
								curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response2);
								res = curl_easy_perform(curl);

								t[t_number] = thread (answers_search, response2, answ1, std::ref(score1), std::ref(mtx));
								if (t[t_number].joinable())	t[t_number].join(); else cout << "Error with thread " << t_number << endl;
								++t_number;

								t[t_number] = thread(answers_search, response2, answ2, std::ref(score2), std::ref(mtx));
								if (t[t_number].joinable())	t[t_number].join(); else cout << "Error with thread " << t_number << endl;
								++t_number;

								t[t_number] = thread(answers_search, response2, answ3, std::ref(score3), std::ref(mtx));
								if (t[t_number].joinable())	t[t_number].join(); else cout << "Error with thread " << t_number << endl;
								++t_number;
							}
						}
					}
					cout << "Runtime = " << (clock() - start) / 1000.0 << " sec" << endl;
				}
				else cout << "CURL Error " << res << endl;
			}
			else if (in == 'q') break;
			cout << endl << endl;
			//_getche();
		}

	}

	curl_easy_cleanup(curl);
	return 0;
}

void answers_search(string response, string answ, int& score, mutex& mtx)
{
	mtx.lock();

	thread t[10];
	string buf;
	int t_number = 1;

	for (size_t i = 0; i < answ.length(); i++)
	{
		while ((answ[i] != ' ' ) && i != answ.length())
		{
			buf += answ[i];
			++i;
		}
		if (!(i != answ.length())) break;

		t[t_number] = thread (kmp_search, response, buf, std::ref(score));
		if (t[t_number].joinable())	t[t_number].join(); else cout << "Error with thread " << t_number << endl;
		++t_number;

		buf = "";
	}

	//cout << endl << answ << ": " << score << endl;
	mtx.unlock();
}

void kmp_search(string response, string answ, int &score)
{
	int Tscore = 0;
	int *offset = new int[answ.size()]{};

	for (int i = 1, j = 0; i < answ.size(); i++)
	{
		while (j && answ[j] != answ[i])
		{
			j = offset[j - 1];
			i++;
		}
		if (answ[j] == answ[i])
		{
			j++;
			offset[i] = answ[j - 1] + 1;
		}
	}

	for (int i = 0, j = 0; i < response.size(); i++)
	{
		while (j && answ[j] != response[i])
		{
			j = offset[j - 1];
			i++;
		}
		if (answ[j] == response[i])
		{
			j++;
		}
		if (j == answ.size())
		{
			Tscore += 1;
		}
	}

	delete[] offset;
	//cout << endl << "\t" << answ << ": " << Tscore << endl;

	score += Tscore;
}

int kmp_search1(string response, string answ)
{
	int Tscore = 0;
	int *offset = new int[answ.size()]{};

	for (int i = 1, j = 0; i < answ.size(); i++)
	{
		while (j && answ[j] != answ[i])
		{
			j = offset[j - 1];
			i++;
		}
		if (answ[j] == answ[i])
		{
			j++;
			offset[i] = answ[j - 1] + 1;
		}
	}

	for (int i = 0, j = 0; i < response.size(); i++)
	{
		while (j && answ[j] != response[i])
		{
			j = offset[j - 1];
			i++;
		}
		if (answ[j] == response[i])
		{
			j++;
		}
		if (j == answ.size())
		{
			Tscore = i;
			break;
		}
	}

	delete[] offset;
	//cout << endl << "\t" << answ << ": " << Tscore << endl;
	return Tscore;
}
