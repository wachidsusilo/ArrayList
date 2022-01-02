#include "ArrayList.h"

template<typename T>
void printArray(ArrayList<T> &list) {
    for (int i = 0; i < list.size(); i++) {
        Serial.print("[" + String(i) + "]: ");
        Serial.println(list[i]);
    }
    Serial.println();
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    //You can create an empty ArrayList
    ArrayList<double> numberList;

    //Or create it with some initial values
    numberList = {1.0, 2.0, 3.0, 4.0, 5.0};

    //Print elements of ArrayList to serial monitor
    Serial.println(">> ArrayList of double:");
    printArray(numberList);

    //Or just create it using existing array
    double numbers[2] = {1.24, 2.24};
    numberList = ArrayList<double>(numbers, 2);

    //Add element to numberList
    numberList.add(6.24);
    numberList.add(43.56);
    numberList.add(35.23);
    numberList.add(21.23);
    numberList.add(11.95);
    numberList.add(12.53);
    numberList.add(7.89);

    //Print elements of ArrayList to serial monitor
    Serial.println("\n>> ArrayList of double:");
    printArray(numberList);

    //Remove the 5th element (21.23) of the list
    Serial.println("\n>> Remove the 6th element (21.23) from the list:");
    numberList.removeByIndex(5);
    printArray(numberList);

    //Remove the first found element which is equal to 12.53
    Serial.println("\n>> Remove the first found element which is equal to 12.53:");
    numberList.remove(12.53);
    printArray(numberList);

    //Remove all elements which is smaller than 3
    Serial.println("\n>> Remove all elements which is smaller than 3:");
    numberList.removeIf([](double n) -> bool { return n < 3; });
    printArray(numberList);

    //Check if the list contains number 4.15
    Serial.println("\n>> Check if the list contains number 4.15:");
    bool exist = numberList.contains(4.15);
    Serial.println(exist ? "true -> Exist" : "false -> Does not exist");

    //Check if numberList contains number greater than 30
    Serial.println("\n>> Check if the list contains number greater than 30:");
    exist = numberList.contains([](double n) -> bool { return n > 30; });
    Serial.println(exist ? "true -> Exist" : "false -> Does not exist");

    //Find the index of number 35.23
    Serial.println("\n>> Find the index of number 35.23:");
    int idx = numberList.indexOf(35.23);
    Serial.println(idx + String(" -> ") + numberList.get(idx, 0.0 /*default value*/));

    //If the number does not exist it will return -1
    Serial.println("\n>> Find the index of number 4.15:");
    idx = numberList.indexOf(4.15);
    Serial.print(idx + String(" -> ") + numberList.get(idx, 0.0 /*default value*/));
    Serial.println(" (doesn't exist)");

    //Find the index of the first element which is greater than 10
    Serial.println("\n>> Find the index of the first element which is greater than 10:");
    idx = numberList.indexOf([](double n) -> bool { return n > 10; });
    Serial.println(idx + String(" -> ") + numberList[idx]);

    //Sort list using default sorting conditional
    Serial.println("\n>> Sort list using default sorting conditional:");
    numberList.sort();
    printArray(numberList);

    //Sort list using custom sorting conditional
    //(a > b) is the default sorting conditional, it's sorted the data ascendingly
    //You can use this function to sort complex struct/class object
    //For example sorting person object by it's age or it's name
    Serial.println("\n>> Sort list using custom sorting conditional:");
    numberList.sort([](double a, double b) -> bool { return a > b; });
    printArray(numberList);

    //Reversing list
    Serial.println("\n>> Reversing list:");
    numberList.reverse();
    printArray(numberList);

    //Filter element which is greater than 7, but smaller than 30
    Serial.println("\n>> Filter element which is greater than 7, but smaller than 30:");
    ArrayList<double> filteredList = numberList.filter([](double n) -> bool { return n > 7 && n < 30; });
    printArray(filteredList);

    //Traditional for loop
    Serial.println("\n>> Traditional for loop:");
    for (int i = 0; i < numberList.size(); i++) {
        Serial.print("[" + String(i) + "]: ");
        Serial.println(numberList[i]);
    }

    //Range based for loop
    Serial.println("\n>> Range based for loop:");
    idx = 0;
    for (auto n : numberList) {
        Serial.print("[" + String(idx++) + "]: ");
        Serial.println(n);
    }

    //Example of using forEach()
    //return true to continue looping, return false to stop looping
    Serial.println("\n>> Example of using forEach():");
    numberList.forEach([](double value, size_t index) -> bool {
        Serial.print("[" + String(index) + "]: ");
        Serial.println(value);
        return true;
    });

    //Example of using map()
    //The old ArrayList would not be altered
    Serial.println("\n>> Example of using map(). Multiply each element by 2:");
    ArrayList<double> newList = numberList.map<double>([](double value, size_t index) -> double { return value * 2; });
    printArray(newList);

    //Mapping to a new ArrayList with different type
    Serial.println("\n>> use map to construct ArrayList<String> from ArrayList<double>:");
    ArrayList<String> stringList = numberList.map<String>([](double value, size_t index) -> String { return String("age: ") + ((int)value) + " years"; });
    printArray(stringList);

    //Clear all of the elements in the list
    Serial.println("\n>> Clear all of the elements in the list. The size of the list would be:");
    numberList.clear();
    Serial.println(numberList.size() + String(" -> ") + (numberList.isEmpty() ? "Empty" : "Not empty"));
}

void loop() {}
