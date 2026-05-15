/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : Occurrence.hpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Date created : 02/03/2026
 * Purpose      : Archivo de cabecera de la clase Occurrence que almacena las coincidencias del texto
 */


#pragma once

#include <string>

class Occurrence {
  private:
    int line_num;
    std::string previous_word;
    std::string next_word;

  public:
    Occurrence(int _line_num, std::string _previous_word, std::string _next_word)
        : line_num(_line_num), previous_word(_previous_word), next_word(_next_word) {}

    int get_line_num() const { return line_num; }
    const std::string &get_previous_word() const { return previous_word; }
    const std::string &get_next_word() const { return next_word; }
    void set_next_word(std::string word) { next_word = std::move(word); }
};