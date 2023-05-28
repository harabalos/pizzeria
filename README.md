# Pizzeria Simulation

## Overview
Pizzeria is a multithreaded pizza order processing simulation implemented in C. It uses threads to represent customers placing orders and the various stages of processing an order, from baking to delivery.

## How to Run

1. **Compile the code**:

    ```sh
    gcc main.c -o pizzeria -pthread
    ```

2. **Run the program**:

    ```sh
    ./pizzeria <Ncust> <seed>
    ```

- `<Ncust>`: Number of customers placing orders.
- `<seed>`: Seed for the random number generator.

## Implementation Details

This program simulates a pizza order processing system with multiple stages. It uses threads to represent each stage of the process. The program uses mutexes to protect shared resources and condition variables to synchronize threads.

For example, each customer is a separate thread. The cook, oven, packer and deliverer are shared resources, each protected by a mutex. Condition variables are used to signal when a resource (like an oven) becomes available.


### Statistics

At the end of the simulation, the program prints out statistics including total profit, number of plain and special pizzas sold, number of successful and failed orders, average and maximum service times, and average and maximum cooling times.


## LICENCE
This project is licensed under the terms of the [MIT](LICENSE) License.
