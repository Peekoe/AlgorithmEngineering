#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <ctype.h>
#include <map>
#include "SensorCluster.hpp"
 
using std::string;
using std::ifstream;
using std::cout;
using std::endl;

/* function to convert a character into its digit; 'A' converts into 10; 'B' into 11, etc.
*/
unsigned int hexa_to_number(char ch) 
{
  unsigned int temp = 0;
  unsigned int list[16]{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  char letters[16]{'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','G'};

  for (size_t i = 0; i < sizeof(letters)/sizeof(letters[0]); i++)
  {
    if (letters[i] == toupper(ch))
    {
      temp = i;
      break;
    }
  }

  return list[temp];
}
    

// function to return the hash value based on the first digit
unsigned int hashfct1(string nic) {
  return hexa_to_number(nic[0]);
}

// function to return the hash value based on the second digit
unsigned int hashfct2(string nic) {
  return hexa_to_number(nic[1]);
}

// function to return the hash value based on the third digit
unsigned int hashfct3(string nic) {
	return hexa_to_number(nic[2]);
}

// function to return the hash value based on the fourth digit
unsigned int hashfct4(string nic) {
	return hexa_to_number(nic[3]);
}

// function to return the hash value based on the fifth digit
unsigned int hashfct5(string nic) {
	return hexa_to_number(nic[4]);
}

// function to return the hash value based on the fourth digit
unsigned int hashfct6(string nic) {
	return hexa_to_number(nic[5]);
}

// Constructor for struct Item
Item::Item(string itemName, string nic):itemName_(itemName), nic_(nic)
{};

// Load information from a text file with the given filename
// THIS FUNCTION IS COMPLETE
void SensorNIC::readTextfile(string filename) {
  ifstream myfile(filename);

  if (myfile.is_open()) {
    cout << "Successfully opened file " << filename << endl;
    string itemName;
    string nic;
    while (myfile >> itemName >> nic) {
			if (itemName.size() > 0)
      	addItem(itemName, nic);
    }
    myfile.close();
  }
  else
    throw std::invalid_argument("Could not open file " + filename);
}

// function that adds the specified NIC to the sensor network (i.e., to all hash tables)
void SensorNIC::addItem(string itemName, string nic) {
  Item item(itemName, nic);
  hT1.insert(std::make_pair(nic, item));
  hT2.insert(std::make_pair(nic, item));
  hT3.insert(std::make_pair(nic, item));
  hT4.insert(std::make_pair(nic, item));
  hT5.insert(std::make_pair(nic, item));
  hT6.insert(std::make_pair(nic, item));
}

// function that removes the sensor specified by the nic value from the network
// if sensor is found, then it is removed and the function returns true
// else returns false
bool SensorNIC::removeItem(string nic) {
  return hT1.erase(nic) + hT2.erase(nic) + hT3.erase(nic) + hT4.erase(nic) + hT5.erase(nic) + hT6.erase(nic);
}

// function that decides the best hash function, i.e. the ones among
// fct1-fct6 that creates the most balanced sensor network for the current
// set of NIC addresses, i.e. data member allItems

// Hints:
// Calculate the balance of each hashtable, one by one.
unsigned int SensorNIC::bestHashing() {
  // map hash digit to min max
  std::map<int, std::pair<int,int>> tables = { 
      {0, {INT32_MAX, 0}}, 
      {1, {INT32_MAX, 0}},
      {2, {INT32_MAX, 0}},
      {3, {INT32_MAX, 0}},
      {4, {INT32_MAX, 0}},
      {5, {INT32_MAX, 0}}
    };

  for(unsigned int i = 0; i < 10; i++)
  {
    int one = hT1.bucket_size(i);
    tables[0].first = std::min(one, tables[0].first);
    tables[0].second = std::max(one, tables[0].second);

    int two = hT2.bucket_size(i);
    tables[1].first = std::min(two, tables[1].first);
    tables[1].second = std::max(two, tables[1].second);

    int three = hT3.bucket_size(i);
    tables[2].first = std::min(three, tables[2].first);
    tables[2].second = std::max(three, tables[2].second);

    int four = hT4.bucket_size(i);
    tables[3].first = std::min(four, tables[3].first);
    tables[3].second = std::max(four, tables[3].second);

    int five = hT5.bucket_size(i);
    tables[4].first = std::min(five, tables[4].first);
    tables[4].second = std::max(five, tables[4].second);

    int six = hT6.bucket_size(i);
    tables[5].first = std::min(six, tables[5].first);
    tables[5].second = std::max(six, tables[5].second);
  }

  /*
  Print for debug/visualization
  for (auto &i : tables)
  {
    printf("\nTable %d Max: %d, Min: %d", i.first + 1, i.second.second, i.second.first);
  }
  */
  int bestIdx = -1, bestDiff = INT32_MAX;

  for(size_t i = 0; i < 6; i++)
  {
    int temp = tables[i].second - tables[i].first;
    if(temp <= bestDiff && temp > 0)
    {
      bestIdx = i;
      bestDiff = temp;
    }
  }

  //printf("\nBest function is %d\n", bestIdx + 1);
    
  return bestIdx + 1;
}

// ALREADY COMPLETED
size_t SensorNIC::size() {
  if ((hT1.size() != hT2.size()) || (hT1.size() != hT3.size()) || (hT1.size() != hT4.size()) || (hT1.size() != hT5.size())|| (hT1.size() != hT6.size()) )
    throw std::length_error("Hash table sizes are not the same");
    
	return hT1.size();
}
