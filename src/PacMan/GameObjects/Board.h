//
// Created by Jakub Szwedowicz on 2/23/25.
//

#ifndef BOARD_H
#define BOARD_H

#include <Entity.h>
#include <memory>
#include <vector>

namespace PacMan {
namespace GameObjects {

class Board {
public:
private:
  std::vector<std::vector<std::unique_ptr<Entities::IEntity>>> m_board;
};

} // namespace GameObjects
} // namespace PacMan

#endif // BOARD_H
