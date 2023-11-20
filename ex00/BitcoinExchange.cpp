/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BitcoinExchange.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maneddam <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/18 17:42:20 by maneddam          #+#    #+#             */
/*   Updated: 2023/11/20 14:02:40 by maneddam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BitcoinExchange.hpp"

void retireveData(std::string line, std::string& y, std::string& m, std::string& d, std::string& v)
{
	int i = 0;
	int dashes = 0;
	int pipe = 0;

	while (line[i] && line[i] != ','&& line[i] != '|')
	{
		if (line[i] != '-' && dashes == 0)
			y += line[i];
		else if (line[i] != '-' && dashes == 1)
			m += line[i];
		else if (line[i] == '|')
			pipe++;
		else if (line[i] != '-' && dashes == 2 && pipe != 1 && line[i] != 32)
			d += line[i];
		if (line[i] == '-')
			dashes++;
		i++;
	}

	// std::cout << line[i] << std::endl;
	while (line[i])
		v += line[++i];
}

bool countPipesDashes(std::string line)
{
	int pipes = 0;
	int dashes = 0;

	pipes = std::count(line.begin(), line.end(), '|');
	dashes = std::count(line.begin(), line.end(), '-');
	if (pipes != 1 || dashes != 2)
		return true;
	return false;
}

bool parseDate(std::string toCheck, int field)
{
	char *end;
	double data;
	data = std::strtod(toCheck.c_str(), &end);
	// if ()
	// std::cout << "[" << toCheck << "]" << std::endl;
	// exit(0);
	switch (field)
	{
	case 0:
		if (*end || data > 2022 || data < 2009)
			return true;
		break;
	case 1:
		if (*end || data > 12 || data <= 0)
			return true;
		break;
	case 2:
		if (*end || data > 31 || data <= 0)
		{
			return true;
		}
		break;
	case 3:
		if (*end || data > 1000 || data < 0)
		{
			// std::cout << "***\n";

			return true;
		}
		break;
	}

	return false;
}

void parseLine(std::string line)
{
	std::string year;
	std::string month;
	std::string day;
	std::string value;
	if (line.empty())
		return ;
	if (countPipesDashes(line))
		std::cout << "Error: bad input => " << line << std::endl;
	else
	{
		retireveData(line, year, month, day, value);

		// std::cout << "[" <<  year << "]"<< std::endl;

		// if (parseDate(year, 0) || parseDate(month, 1) || parseDate(day, 2) || parseDate(value, 3))
		if (parseDate(year, 0) || parseDate(month, 1) || parseDate(day, 2) || parseDate(value, 3))
			std::cout << "Error: bad input => " << line << std::endl;
		else
			std::cout << line << std::endl;

		// std::cout << year << "-" << month << "-" << day  << value << std::endl;
	}
}

void parseTheFile(std::string toOpen)
{
	std::fstream obj;
	std::string line;

	obj.open(toOpen.c_str(), std::ios::in);
	if (obj.is_open())
	{
		getline(obj, line);
		while (line.empty())
			getline(obj, line);

		if (line != "date | value")
			throw std::runtime_error("Invalid header line");
		else
		{
			while (1)
			{
				if (getline(obj, line).eof())
					break;
				parseLine(line);
			}
		}
	}
	else
		throw std::runtime_error("Error: could not open file.");
}

void insertLineIntoDB(std::string year, std::string month, std::string day, std::string value, std::map<unsigned long, double>& mydb)
{
	std::string key = year + month + day;
	char *end;
	char *end1;
	double v1;
	double v2;

	v1 = std::strtod(key.c_str(), &end);
	v2 = std::strtod(value.c_str(), &end1);


	mydb.insert(std::pair<unsigned long, double>(static_cast<unsigned long>(v1), v2));

}

void insertIntoDB(std::map<unsigned long, double>& mydb, std::string line)
{
	std::string year;
	std::string month;
	std::string day;
	std::string value;

	retireveData(line, year, month, day, value);
	insertLineIntoDB(year, month, day, value, mydb);
}

void loadData(std::map<unsigned long, double>& mydb)
{
	std::fstream obj;
	std::string line;
	obj.open("data.csv", std::ios::in);
	if (obj.is_open())
	{
		getline(obj, line);
		while (1)
		{
			if (getline(obj, line).eof())
				break;
			insertIntoDB(mydb, line);
		}
		// for (std::map<unsigned long, double>::iterator it = mydb.begin(); it != mydb.end() ; ++it)
		// {
		// 	std::cout << it->first << "," << it->second << std::endl;
		// }
	}
	else
		throw std::runtime_error("Error: could not open file.");
}
