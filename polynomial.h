//
// Created by nickr on 2022-03-02.
//

#ifndef ROOTFINDER_POLYNOMIAL_H
#define ROOTFINDER_POLYNOMIAL_H

#define SMALLSTEP 0.001
#define BADPDIV 0.1

#include <iostream>
#include <vector>
#include <cmath>
#include "complex.h"
#include "guessMatrix.h"

// INFO
//   Implements polynomial class alongside related methods and
//   to use newtons method in its internal guessMatrix obj to find roots of the above polynomial

class polynomial {
public:
    int order;
    int badpdiv = 0;
    std::vector<complex> pml;
    std::vector<complex> derivative;
    std::vector<complex> rootList;
    std::vector<complex> officialRootList;
    guessMatrix gm;
    guessMatrix roots;

    void initPML() {
        std::cout << "Enter Order: ";
        std::cin >> this->order;
        std::cout << "\n";
        std::cout << "Enter Terms Highest to Lowest Order\n x^2 + 3x + 2 --> 1, 3, 2\n";
        complex term;
        this->gm.initgm(10, 0.05);


        for (int i = 0; i <= order; i++) {
            std::cout << "Real Part of Term: ";
            std::cin >> term.real;
            pml.push_back(term);
        }

        for (int i = 0; i < this->pml.size() - 1; i++) {
            this->derivative.push_back(scalar(this->pml.size() - 1 - i, pml[i]));
        }
    }

    void printpml() {
        for (int i = 0; i < this->pml.size(); i++) {
            pml[i].printz();
        }
    }

    void printderiv() {
        for (int i = 0; i < this->derivative.size(); i++) {
            derivative[i].printz();
        }
    }

    complex pmlvalue(complex z) {
        // REQUIRES
        //
        // PROMISES
        //   Returns evaluation of objects internal polynomial at point z

        complex value;
        value.real = 0;
        value.imag = 0;
        for (int i = 0; i < this->pml.size(); i++) {
            value = addition(mult(exp(this->pml.size() - 1 - i, z), pml[i]), value);
        }
        return value;
    }
    complex derivvalue(complex z) {
        // REQUIRES
        //
        // PROMISES
        //   Returns evaluation of objects internal polynomial derivative at point z

        complex value;
        value.real = 0;
        value.imag = 0;
        for (int i = 0; i < this->derivative.size(); i++) {
            value = addition(mult(exp(this->derivative.size() - 1 - i, z), derivative[i]), value);
        }
        return value;
    }

    complex newtonsGuess(complex z0) { // Test if P'(z0) = 0
        // REQUIRES
        //
        // PROMISES
        //   Returns newtons iteration on given complex number
        //   Returns same number with bad iteration flag when you would divide by 0 in newtons method
        //   Also sets smallStepFlag to 1 with a sufficiently small iteration (watching for convergence)

        complex z1;
        complex polyPrimeVal = derivvalue(z0);
        complex polyVal = pmlvalue(z0);
        double margin = 5;
        if (mag(div(polyVal, polyPrimeVal)) > margin) {
            z1.badIteration = 1;
            return z1;
        }
        z1 = addition(z0, scalar(-1, div(polyVal, polyPrimeVal)));
        if (mag(scalar(-1, div(polyVal, polyPrimeVal))) < SMALLSTEP) {
            z1.smallStep = 1;
        }
        return z1;
    }
    void iterateGuessMatrix() {
        // REQUIRES
        //
        // PROMISES
        //   Performs newtons iteration on guessMatrix
        //   Also removes bad iteration steps


        int j = 0;
        for (int i = 0; i < this->gm.guesslist.size(); i++) {
            complex z1;
            z1 = newtonsGuess(this->gm.guesslist[i - j]);
            if (err(z1)) {
                this->gm.guesslist.erase(this->gm.guesslist.begin() + i - j);
                j++;
            }
            else {
                this->gm.guesslist[i - j] = z1;
            }
        }

    }

    void convergenceTest() {
        // REQUIRES
        //
        // PROMISES
        //   Adds numbers with small step size to new guessMatrix

        complex root;
        complex iteration;
        root.real = 0;
        root.imag = 0;
        iteration.real = 0;
        iteration.imag = 0;
        for (int i = 0; i < this->gm.guesslist.size(); i++) {
            if (this->gm.guesslist[i].smallStep == 1) {             //Finds first small iteration in list
                root = this->gm.guesslist[i];                       //Small Number close to root
                this->roots.guesslist.push_back(root);
            }
        }
    }

    void printgm() {
        for (int i = 0; i < this->gm.guesslist.size(); i++) {
            this->gm.guesslist[i].printz();
        }
    }

    complex findARoot(complex z0) {
        // REQUIRES
        //
        // PROMISES
        //   Performs newtowns iteration multiple times on single complex number for more precision
        //   Stops when step size is less than a predefined prescision

        double precision = 0.000001;
        double stepSize = 1;
        complex prevGuess;
        complex currentGuess = z0;

        while (stepSize > precision) {
            prevGuess = currentGuess;
            currentGuess = newtonsGuess(currentGuess); //Watch for
            if (currentGuess.badIteration == 1) {
                //break?
            }
            stepSize = mag(sub(currentGuess, prevGuess));
        }
        return currentGuess;
    }

    complex pruneListFromRoot(complex z, double radius) {
        // REQUIRES
        //
        // PROMISES
        //   Removes every root from list not close enough to given root
        //   Given root found from above findARoot method

        int j = 0;
        for (int i = 0; i < this->roots.guesslist.size(); i++) {
            if (mag(sub(z, this->roots.guesslist[i - j])) < radius) {
                this->roots.guesslist.erase(this->roots.guesslist.begin() + i - j);
                j++;
            }
        }
        return z;
    }

    void cleanRoots() {
        // REQUIRES
        //
        // PROMISES
        //   Performs pruneListFromRoot method until it has only the same number of roots as its order
        //   sets final rootList equal to second guessMatrix's internal list

        complex root;
        for (int i = 0; this->roots.guesslist.size() > order; i++) {
            root = findARoot(this->roots.guesslist[0]);
            this->roots.guesslist.push_back(this->pruneListFromRoot(root, 0.1));
        }
        this->rootList = this->roots.guesslist;
    }

    void printRoots() {
        for (int i = 0; i < this->rootList.size(); i++) {
            this->rootList[i].printz();
        }
    }
};


#endif //ROOTFINDER_POLYNOMIAL_H
