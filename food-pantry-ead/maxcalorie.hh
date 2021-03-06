////////////////////////////////////////////////////////////////////////////////
// maxcalorie.hh
//
// Compute the set of foods that maximizes the calories in foods, within 
// a given maximum weight with the dynamic programming or exhaustive search.
//
///////////////////////////////////////////////////////////////////////////////


#pragma once


#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>


// One food item available for purchase.
class FoodItem
{
	//
	public:
		
		//
		FoodItem
		(
			const std::string& description,
			double weight_ounces,
			double calories
		)
			:
			_description(description),
			_weight_ounces(weight_ounces),
			_calories(calories)
		{
			assert(!description.empty());
			assert(weight_ounces > 0);
		}
		
		//
		const std::string& description() const { return _description; }
		double weight() const { return _weight_ounces; }
		double foodCalories() const { return _calories; }
	
	//
	private:
		
		// Human-readable description of the food, e.g. "spicy chicken breast". Must be non-empty.
		std::string _description;
		
		// Food weight, in ounces; Must be positive
		double _weight_ounces;
		
		// Calories; most be non-negative.
		double _calories;
};


// Alias for a vector of shared pointers to FoodItem objects.
typedef std::vector<std::shared_ptr<FoodItem>> FoodVector;


// Load all the valid food items from the CSV database
// Food items that are missing fields, or have invalid values, are skipped.
// Returns nullptr on I/O error.
std::unique_ptr<FoodVector> load_food_database(const std::string& path)
{
	std::unique_ptr<FoodVector> failure(nullptr);
	
	std::ifstream f(path);
	if (!f)
	{
		std::cout << "Failed to load food database; cannot open file: " << path << std::endl;
		return failure;
	}
	
	std::unique_ptr<FoodVector> result(new FoodVector);
	
	size_t line_number = 0;
	for (std::string line; std::getline(f, line); )
	{
		line_number++;
		
		// First line is a header row
		if ( line_number == 1 )
		{
			continue;
		}
		
		std::vector<std::string> fields;
		std::stringstream ss(line);
		
		for (std::string field; std::getline(ss, field, '^'); )
		{
			fields.push_back(field);
		}
		
		if (fields.size() != 3)
		{
			std::cout
				<< "Failed to load food database: Invalid field count at line " << line_number << "; Want 3 but got " << fields.size() << std::endl
				<< "Line: " << line << std::endl
				;
			return failure;
		}
		
		std::string
			descr_field = fields[0],
			weight_ounces_field = fields[1],
			calories_field = fields[2]
			;
		
		auto parse_dbl = [](const std::string& field, double& output)
		{
			std::stringstream ss(field);
			if ( ! ss )
			{
				return false;
			}
			
			ss >> output;
			
			return true;
		};
		
		std::string description(descr_field);
		double weight_ounces, calories;
		if (
			parse_dbl(weight_ounces_field, weight_ounces)
			&& parse_dbl(calories_field, calories)
		)
		{
			result->push_back(
				std::shared_ptr<FoodItem>(
					new FoodItem(
						description,
						weight_ounces,
						calories
					)
				)
			);
		}
	}

	f.close();
	
	return result;
}


// Convenience function to compute the total weight and calories in 
// a FoodVector.
// Provide the FoodVector as the first argument
// The next two arguments will return the weight and calories back to 
// the caller.
void sum_food_vector
(
	const FoodVector& foods,
	double& total_weight,
	double& total_calories
)
{
	total_weight = total_calories = 0;
	for (auto& food : foods)
	{
		total_weight += food->weight();
		total_calories += food->foodCalories();
	}
}


// Convenience function to print out each FoodItem in a FoodVector,
// followed by the total weight and calories of it.
void print_food_vector(const FoodVector& foods)
{
	std::cout << "*** food Vector ***" << std::endl;
	
	if ( foods.size() == 0 )
	{
		std::cout << "[empty food list]" << std::endl;
	}
	else
	{
		for (auto& food : foods)
		{
			std::cout
				<< "Ye olde " << food->description()
				<< " ==> "
				<< "Weight of " << food->weight() << " ounces"
				<< "; calories = " << food->foodCalories()
				<< std::endl
				;
		}
		
		double total_weight, total_calories;
		sum_food_vector(foods, total_weight, total_calories);
		std::cout
			<< "> Grand total weight: " << total_weight << " ounces" << std::endl
			<< "> Grand total calories: " << total_calories
			<< std::endl
			;
	}
}


// Filter the vector source, i.e. create and return a new FoodVector
// containing the subset of the food items in source that match given
// criteria.
// This is intended to:
//	1) filter out food with zero or negative calories that are irrelevant to // our optimization
//	2) limit the size of inputs to the exhaustive search algorithm since it // will probably be slow.
//
// Each food item that is included must have at minimum min_calories and 
// at most max_calories.
//	(i.e., each included food item's calories must be between min_calories
// and max_calories (inclusive).
//
// In addition, the the vector includes only the first total_size food items
// that match these criteria.
std::unique_ptr<FoodVector> filter_food_vector
(
	const FoodVector& source,
	double min_calories,
	double max_calories,
	int total_size
)
{
	// each item must be in range [minCalories,maxCalories]
	std::unique_ptr<FoodVector> filteredFoods(new FoodVector);
	unsigned int selectedFoods = 0;

	for (auto& food : source)
	{
		if (selectedFoods <= total_size && // Make sure we haven't picked too many foods
		(food->foodCalories() >= min_calories && food->foodCalories() <= max_calories)) // also make sure its in the range
		{
			filteredFoods->push_back(food);
			if (++selectedFoods >= total_size)
				break;
		}
	}

	return filteredFoods;
}

// Compute the optimal set of food items with a exhaustive search algorithm.
// Specifically, among all subsets of food items, return the subset 
// whose weight in ounces fits within the total_weight one can carry and
// whose total calories is greatest.
// To avoid overflow, the size of the food items vector must be less than 64.
std::unique_ptr<FoodVector> exhaustive_max_calories
(
	const FoodVector& foods,
	double total_weight
)
{
	/*
	exhaustive_max_time(W, food_items):
		n = |food_items|
		best = None
		for bits from 0 to (2n -1):
			candidate = empty vector
			for j from 0 to n-1:
				if ((bits >> j) & 1) == 1:
					candidate.add_back(food_items[j])
			if total_weight(candidate) <= W:
				if best is None or
					total_calories(candidate) > total_calories(best):
						best = candidate
	return best
	*/

	unsigned int n = foods.size();
	std::unique_ptr<FoodVector> best(new FoodVector); 
	double bestWeightSum, bestTotalCalories = 0;
	
	for (uint64_t i = 0; i < pow(2,n); i++)
	{
		FoodVector candidate;
		for (unsigned int j = 0; j < n; j++)
		{
			if(((i >> j) & 1) == 1) // bitwise Subset generation
			{
				candidate.push_back(foods[j]);
			}
		}
		
		double calorieSum, weightSum = 0;
		

		
		sum_food_vector(candidate,weightSum,calorieSum); // store the candidate calculation into variable
		
		if (weightSum <= total_weight)
		{
			if (best->empty() || (calorieSum > bestTotalCalories))
			{
				*best = candidate;
				sum_food_vector(*best, bestWeightSum, bestTotalCalories); // update computation of best's new values
			}
		}

	}


	return best;

}




// Compute the optimal set of food items with dynamic programming.
// Specifically, among the food items that fit within a total_weight,
// choose the foods whose calories-per-weight is greatest.
// Repeat until no more food items can be chosen, either because we've 
// run out of food items, or run out of space.
std::unique_ptr<FoodVector> dynamic_max_calories
(
	const FoodVector& foods,
	double total_weight // ASSUME it is an int
)
{
	const int W = total_weight;

	std::unique_ptr<FoodVector> source(new FoodVector(foods)); // input
	const int n = source->size();

	std::vector<std::vector<double>> T(n+1, std::vector<double>((W + 1))); // DP Table
	std::unique_ptr<FoodVector> best(new FoodVector); // solution

	for (int init = 1; init <= n; init++)
	{
		for (int j = 1; j <= W; j++)
		{
			T[init][j] = -1000;		// fill in negative weights
		}
	}

	for (int j = 0; j <= W; j++)
		T[0][j] = 0;	// fill in zeros for row Zero
	for (int i = 0; i <= n; i++)
		T[i][0] = 0;	// fill in zeros for col Zero

	for (int i = 1; i <= n; i++)
	{
		for (int j = 0; j <= W; j++)
		{
			
			double maxTopDefault = T[i-1][j];
			double maxTentative = 0; // We initialize this value to 0

			int currentWeight = source->operator[](i-1)->weight(); // We use index -1 to account for the extra row at the top

			if (j >= currentWeight) // Does the knapsack fit our current item?
			{ 
				maxTentative = source->operator[](i-1)->foodCalories(); // If so, maxTentative is at least the value of the current item

				int remainingCapacity = j - currentWeight; 
				maxTentative += T[i - 1][remainingCapacity]; 
			}

			T[i][j] = std::max(maxTopDefault, maxTentative); // Pick the larger of the two
		}
	}

	int i = n;
	int j = W;
	while (i > 0) 
	{
		if (T[i][j] != T[i - 1][j]) 
		{
			j -= source->operator[](i - 1)->weight();
			best->push_back(source->operator[](i - 1));
		}
		i -= 1;
	}
	//print_food_vector(*best);
	return best;
}