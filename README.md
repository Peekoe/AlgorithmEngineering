# Algorithm Engineering
Class projects from my Summer CPSC 335 class, more detailed analysis can be found in the reports in each project folder.

### Project 1: Alternating Disks

The problem statement for this project is:
> You have a row of 2n disks of two colors, n dark and n light. They alternate: dark, light, dark, light, and so on. You want to get all the dark disks to the right hand end, and all the light disks to the left hand end. The only moves you are allowed to make are those that interchange the positions of two neighboring disks. Design an algorithm for solving this puzzle and determine the number of moves it takes.

We implemented two different algorithms to solve this problem, the lawnmower algorithm and the alternate algorithm. Lawnmower would go across the disks from left to right, then right to left switching disks where needed. The alternate algorithm would go left to right and swap disks where needed, then after that iteration would start again but at the next index of the list, ex. 0 - n, 1 - n, ... n-1 - n.

### Project 2: Food Pantry (Greedy and Exhaustive)
The problem statement for this project is:
> You need to gain as many calories from the food you obtain from a food pantry. You have a limited weight you can carry on your own to your house and that will limit as much food items you can pick up from the food pantry. There are many different types of food product you can pick up, each has a certain weight in ounces and a calorie content. You need to maximize the amount of calories to stave off your hunger.

This project also required implementing two algorithms, a greedy approach taking the items with the best calorie-to-weight ratio first until no more could be carried, and an exhaustive approach that generated every subset of the food items given then selected the one that provided the most calories.

### Project 3: Network Clustering
The problem statement for this project is:
> Your project is about reading a fairly small number of distinct Network Interface Controllers (NICs), each being a 6-digit number in hexadecimal, and deciding which digit among the six gives the best balanced distribution of the NICs. Thus one can consider a MAC address as a 6-alphanumeric string from the set.

In this we took the IDs of these sensors and used each digit of them to determine which one created the most balanced hash table.

### Project 4: Food Pantry (Exhasutive and Dynamic)

This project uses the same problem as Project 2 but instead of taking a greedy approach, we were asked to use Dynamic Programming to find an optimal solution to this knapsack problem.
