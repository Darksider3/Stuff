//
// Created by darksider3 on 19.04.21.
//
#include <bits/stdc++.h>
#include <fmt/core.h>
#include <iostream>

const std::string GENES = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOP"
                          "QRSTUVWXYZ 1234567890, .-;:_!\"#%&/()=?@${[]}";

constexpr size_t POPULATION_SIZE = 300;
constexpr double childSelectorThreshold = 0.50;
constexpr double ParentSelectorThreshold = 0.99;
std::string TARGET = "Something here right now!";

int random_num(int start, int end)
{
    int range = (end - start) + 1;
    int random_int = start + (rand() % range);

    return random_int;
}

char mutated_genes()
{
    int len = GENES.size();
    int r = random_num(0, len - 1);
    return GENES[r];
}

std::string create_genome()
{
    int len = TARGET.size();
    std::string genome = "";
    for (int i = 0; i < len; ++i)
        genome += mutated_genes();

    return genome;
}

class Individual {
private:
    std::string m_chromosome;
    size_t m_fitness;

public:
    Individual(const std::string& chromosome);
    Individual mate(Individual Parent);
    int cal_fitness();
    friend bool operator<(const Individual& ind1, const Individual& ind2);

    [[nodiscard]] inline size_t getFitness() const { return m_fitness; }
    [[nodiscard]] inline std::string getChromosome() const { return m_chromosome; }
};
Individual::Individual(const std::string& chromosome)
{
    m_chromosome = chromosome;
    m_fitness = cal_fitness();
}

Individual Individual::mate(Individual Parent)
{
    std::string child_chrom = "";
    int len = m_chromosome.size();

    for (int i = 0; i < len; ++i) {
        float p = random_num(0, 100) / 100;

        if (p < childSelectorThreshold)
            child_chrom += m_chromosome[i];
        else if (p < ParentSelectorThreshold)
            child_chrom += Parent.m_chromosome[i];
        else
            child_chrom += mutated_genes();
    }

    return Individual(child_chrom);
}

// Calculate fittness score, it is the number of
// characters in string which differ from target
// string.
int Individual::cal_fitness()
{
    int len = TARGET.size();
    int fitness = 0;
    for (int i = 0; i < len; i++) {
        if (m_chromosome[i] != TARGET[i])
            fitness++;
    }
    return fitness;
};

// Overloading < operator
bool operator<(const Individual& ind1, const Individual& ind2)
{
    return ind1.m_fitness < ind2.m_fitness;
}

// Driver code
int main()
{
    srand((unsigned)(time(0)));

    // current generation
    int generation = 0;

    std::vector<Individual> population;
    bool found = false;

    // create initial population
    for (size_t i = 0; i < POPULATION_SIZE; i++) {
        std::string gnome = create_genome();
        population.emplace_back(gnome);
    }

    while (!found) {
        // sort the population in increasing order of m_fitness score
        sort(population.begin(), population.end());

        // if the individual having lowest m_fitness score ie.
        // 0 then we know that we have reached to the target
        // and break the loop
        if (population[0].getFitness() <= 0) {
            found = true;
            break;
        }

        // Otherwise generate new offsprings for new generation
        std::vector<Individual> new_generation;

        // Perform Elitism, that mean 10% of fittest population
        // goes to the next generation
        size_t s = (10 * POPULATION_SIZE) / 100;
        new_generation.reserve(s);
        for (size_t i = 0; i < s; i++)
            new_generation.push_back(population[i]);

        // From 50% of fittest population, Individuals
        // will mate to produce offspring
        s = (90 * POPULATION_SIZE) / 100;
        for (size_t i = 0; i < s; i++) {
            int r = random_num(0, 50);
            Individual parent1 = population[r];
            r = random_num(0, 50);
            Individual parent2 = population[r];
            Individual offspring = parent1.mate(parent2);
            new_generation.push_back(offspring);
        }
        population = new_generation;
        fmt::print("Generation: {}\tString: {}\t", generation, population[0].getChromosome());
        fmt::print("Fitness: {}\n", population[0].getFitness());

        generation++;
    }
    std::cout << "Generation: " << generation << "\t";
    std::cout << "String: " << population[0].getChromosome() << "\t";
    std::cout << "Fitness: " << population[0].getFitness() << "\n";
}