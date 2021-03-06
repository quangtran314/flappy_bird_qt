#include "physics.h"

#include <QMainWindow>
#include <QGraphicsScene>
#include <QTimer>
#include <QTransform>
#include <QParallelAnimationGroup>

#include "game/game.h"
#include "bird/bird.h"
#include "scene/scene.h"


Physics::Physics(Game *parent_game, int tickRate, bool isOnlyGround, qreal speedFactor, bool isCollisionDetectionDisabled)
 :  speedfactor(speedFactor), collisionDetectionDisabled(isCollisionDetectionDisabled), game(parent_game), onlyGround(isOnlyGround), updateInterval(tickRate)
{
    _transform = new QTransform();

    bird = game->scene->bird;

    pipeMarkers[0] = 0;
    pipeMarkers[1] = 0;
    pipeMarkers[2] = 0;
    pipeMarkers[3] = 0;


    markers[0] = 0;
    markers[1] = 0;
    markers[2] = 0;


    game->scene->pipe[0][0]->setPos(game->getScreenWidth(), 0);
    game->scene->pipe[0][1]->setPos(game->getScreenWidth(), 0);
    game->scene->pipe[0][2]->setPos(game->getScreenWidth(), 0);
    game->scene->pipe[1][0]->setPos(game->getScreenWidth(), 0);
    game->scene->pipe[1][1]->setPos(game->getScreenWidth(), 0);
    game->scene->pipe[1][2]->setPos(game->getScreenWidth(), 0);

    physicsTimer = new QTimer();

    QObject::connect(physicsTimer, &QTimer::timeout, [this](){
        if(!onlyGround)
        {
            if(collisionCheck())
            {
                bird->fall();
                game->gameOver();
                return;
            }
            movePipes();
        }
        moveGround();
        QApplication::processEvents();
    });

    physicsTimer->setInterval(onlyGround ? updateInterval * PHYSICS_ONLYGROUND_SLOW_RATE : updateInterval);
    physicsTimer->start();
}

Physics::~Physics()
{
    delete _transform;
    delete physicsTimer;
}

void Physics::stop()
{
    physicsTimer->stop();
}

void Physics::start()
{
    physicsTimer->start();
}

void Physics::moveGround()
{
    game->scene->updateGround();
    game->scene->ground->setPos(game->scene->ground->pos().x() - (PHYSICS_UNIT_MOVE_RATE(game->getScreenWidth()) * speedfactor), game->scene->ground->pos().y());
}

bool Physics::isOnlyGround()
{
    return onlyGround;
}

bool Physics::collisionCheck()
{
    bool bTrue = true;

    if(collisionDetectionDisabled)
        bTrue = false;

    birdRect.x = bird->x();
    birdRect.y = bird->y();
    birdRect.width = bird->x() + bird->boundingRect().width();
    birdRect.height = bird->y() + bird->boundingRect().height();

    if(((birdRect.width > game->scene->pipe[1][1]->pos().x()) &&
        (birdRect.width < game->scene->pipe[1][1]->boundingRect().width() + game->scene->pipe[1][1]->pos().x())) &&
            (birdRect.height > game->scene->pipe[1][1]->pos().y() ||
             birdRect.height < game->scene->pipe[0][1]->pos().y() + game->scene->pipe[0][1]->boundingRect().height()))
    {
            return bTrue;
    }
    else if((birdRect.width >= game->scene->pipe[1][1]->pos().x() &&
             birdRect.x <= game->scene->pipe[1][1]->pos().x() + game->scene->pipe[1][1]->boundingRect().width()) &&
            (birdRect.height > game->scene->pipe[1][1]->pos().y() ||
             birdRect.y < game->scene->pipe[0][1]->pos().y() + game->scene->pipe[0][1]->boundingRect().height()))
    {
            return bTrue;
    }

    if(((birdRect.width > game->scene->pipe[1][2]->pos().x()) &&
        (birdRect.width < game->scene->pipe[1][2]->boundingRect().width() + game->scene->pipe[1][2]->pos().x())) &&
            (birdRect.height > game->scene->pipe[1][2]->pos().y() ||
             birdRect.height < game->scene->pipe[0][2]->pos().y() + game->scene->pipe[0][2]->boundingRect().height()))
    {
            return bTrue;
    }
    else if((birdRect.width >= game->scene->pipe[1][2]->pos().x() &&
             birdRect.x <= game->scene->pipe[1][2]->pos().x() + game->scene->pipe[1][2]->boundingRect().width()) &&
            (birdRect.height > game->scene->pipe[1][2]->pos().y() ||
             birdRect.y < game->scene->pipe[0][2]->pos().y() + game->scene->pipe[0][2]->boundingRect().height()))
    {
            return bTrue;
    }

    if(((birdRect.width > game->scene->pipe[1][0]->pos().x()) &&
        (birdRect.width < game->scene->pipe[1][0]->boundingRect().width() + game->scene->pipe[1][0]->pos().x())) &&
            (birdRect.height > game->scene->pipe[1][0]->pos().y() ||
             birdRect.height < game->scene->pipe[0][0]->pos().y() + game->scene->pipe[0][0]->boundingRect().height()))
    {
            return bTrue;
    }
    else if((birdRect.width >= game->scene->pipe[1][0]->pos().x() &&
             birdRect.x <= game->scene->pipe[1][0]->pos().x() + game->scene->pipe[1][0]->boundingRect().width()) &&
            (birdRect.height > game->scene->pipe[1][0]->pos().y() ||
             birdRect.y < game->scene->pipe[0][0]->pos().y() + game->scene->pipe[0][0]->boundingRect().height()))
    {
            return bTrue;
    }



    if((birdRect.y < -bird->boundingRect().height()) || (birdRect.height > game->scene->ground->pos().y()))
    {
        return bTrue;
    }



    if((birdRect.x >= game->scene->pipe[1][0]->pos().x() + game->scene->pipe[1][0]->boundingRect().width()) &&
            markers[0] == 0 && (birdRect.width < game->scene->pipe[1][1]->pos().x()))
    {
        game->updateScore();
        markers[0] = 1;
        markers[2] = 0;
    }
    else if((birdRect.x >= game->scene->pipe[1][1]->pos().x() + game->scene->pipe[1][1]->boundingRect().width()) &&
            markers[1] == 0 && (birdRect.width < game->scene->pipe[1][2]->pos().x()))
    {
        game->updateScore();
        markers[0] = 0;
        markers[1] = 1;
    }
    else if((birdRect.x >= game->scene->pipe[1][2]->pos().x() + game->scene->pipe[1][2]->boundingRect().width()) &&
            markers[2] == 0 && (birdRect.width < game->scene->pipe[1][0]->pos().x()))
    {
        game->updateScore();
        markers[1] = 0;
        markers[2] = 1;
    }

    return false;
}

int Physics::randInt(int low, int high)
{
    return qrand() % ((high + 1) - low) + low;
}

void Physics::movePipes()
{
    if((game->scene->pipe[1][1]->pos().x() >= game->getScreenWidth()) && (pipeMarkers[0] == 0))
    {

        if(pipeMarkers[1])
        {
            game->scene->pipe[1][1]->setPos(game->scene->pipe[1][2]->pos().x() + (game->scene->pipe[1][1]->boundingRect().width()) + (game->getScreenWidth() / 2.6),randInt(game->getScreenHeight()/3.7,game->getScreenHeight() / 1.4));
        }
        else
        {
            game->scene->pipe[1][1]->setPos(game->getScreenWidth() + (game->scene->pipe[1][1]->boundingRect().width()),randInt(game->getScreenHeight() / 3.5,game->getScreenHeight() / 1.4));
        }
        game->scene->pipe[0][1]->setPos(game->scene->pipe[1][1]->pos().x(),game->scene->pipe[1][1]->pos().y() - (game->getScreenHeight() / 5.05) - game->scene->pipe[1][1]->boundingRect().height());
        pipeMarkers[0] = 1;
    }
    if((game->scene->pipe[1][0]->pos().x() >= game->getScreenWidth()) && (pipeMarkers[2] == 0))
    {
        if(pipeMarkers[1])
        {
            game->scene->pipe[1][0]->setPos(game->scene->pipe[1][1]->pos().x() + game->scene->pipe[1][0]->boundingRect().width() + (game->getScreenWidth() / 2.6),randInt(game->getScreenHeight()/3.7,game->getScreenHeight() / 1.4));
        }
        else
        {
            game->scene->pipe[1][0]->setPos(game->scene->pipe[1][1]->pos().x() + (game->getScreenWidth() / 2.6) + (game->scene->pipe[1][0]->boundingRect().width()),randInt(game->getScreenHeight() / 3.7,game->getScreenHeight() / 1.4));
        }
        game->scene->pipe[0][0]->setPos(game->scene->pipe[1][0]->pos().x(),game->scene->pipe[1][0]->pos().y() - (game->getScreenHeight() / 5.05) - game->scene->pipe[1][0]->boundingRect().height());
        pipeMarkers[2] = 1;
    }
    if(game->scene->pipe[1][2]->pos().x() >= game->getScreenWidth() && (pipeMarkers[3] == 0))
    {
        if(pipeMarkers[1])
        {
            game->scene->pipe[1][2]->setPos(game->scene->pipe[1][0]->pos().x() + game->scene->pipe[1][2]->boundingRect().width() + (game->getScreenWidth() / 2.6),randInt(game->getScreenHeight() / 3.7,game->getScreenHeight() / 1.4));
        }
        else
        {
            game->scene->pipe[1][2]->setPos(game->scene->pipe[1][0]->pos().x() + (game->getScreenWidth() / 2.6) + (game->scene->pipe[1][2]->boundingRect().width()),randInt(game->getScreenHeight() / 3.7,game->getScreenHeight() / 1.4));
        }
        game->scene->pipe[0][2]->setPos(game->scene->pipe[1][2]->pos().x(),game->scene->pipe[1][2]->pos().y() - (game->getScreenHeight() / 5.05) - game->scene->pipe[0][2]->boundingRect().height());
        pipeMarkers[3] = 1;
    }

    game->scene->pipe[1][1]->setPos(game->scene->pipe[1][1]->pos().x() - (PHYSICS_UNIT_MOVE_RATE(game->getScreenWidth()) * speedfactor), game->scene->pipe[1][1]->y());
    game->scene->pipe[0][1]->setPos(game->scene->pipe[0][1]->pos().x() - (PHYSICS_UNIT_MOVE_RATE(game->getScreenWidth()) * speedfactor), game->scene->pipe[0][1]->y());
    game->scene->pipe[1][0]->setPos(game->scene->pipe[1][0]->pos().x() - (PHYSICS_UNIT_MOVE_RATE(game->getScreenWidth()) * speedfactor), game->scene->pipe[1][0]->y());
    game->scene->pipe[0][0]->setPos(game->scene->pipe[0][0]->pos().x() - (PHYSICS_UNIT_MOVE_RATE(game->getScreenWidth()) * speedfactor), game->scene->pipe[0][0]->y());
    game->scene->pipe[1][2]->setPos(game->scene->pipe[1][2]->pos().x() - (PHYSICS_UNIT_MOVE_RATE(game->getScreenWidth()) * speedfactor), game->scene->pipe[1][2]->y());
    game->scene->pipe[0][2]->setPos(game->scene->pipe[0][2]->pos().x() - (PHYSICS_UNIT_MOVE_RATE(game->getScreenWidth()) * speedfactor), game->scene->pipe[0][2]->y());

    if(game->scene->pipe[1][1]->pos().x() < -game->scene->pipe[1][1]->boundingRect().width())
    {
        pipeMarkers[0] = 0;
        pipeMarkers[1] = 1;
        game->scene->pipe[1][1]->setPos(game->getScreenWidth(), 0);
    }
    if(game->scene->pipe[1][0]->pos().x() < -game->scene->pipe[1][0]->boundingRect().width())
    {
        pipeMarkers[2] = 0;
        pipeMarkers[1] = 1;
        game->scene->pipe[1][0]->setPos(game->getScreenWidth(), 0);
    }
    if(game->scene->pipe[1][2]->pos().x() < -game->scene->pipe[1][2]->boundingRect().width())
    {
        pipeMarkers[3] = 0;
        pipeMarkers[1] = 1;
        game->scene->pipe[1][2]->setPos(game->getScreenWidth(), 0);
    }
}

void Physics::switchOnlyGroundMove()
{
    physicsTimer->setInterval(onlyGround ? updateInterval : updateInterval * PHYSICS_ONLYGROUND_SLOW_RATE);
    onlyGround = !onlyGround;
}
