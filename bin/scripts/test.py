# Это надо будет вынести куда-то в общее место
class FuryBehaviour:
    def __init__(self, _object):
        print('init')
        self.object = _object


# Модуль начинается тут
import os
import numpy as np
import tensorflow as tf

from scripts.discrete_deepq import DiscreteDeepQ
from scripts.model import MLP


class Behaviour(FuryBehaviour):
    def start(self):
        self.isFirst = True
        self.score = 0
        self.gameCounter = 0
        
        self.observation_size = 49
        self.num_actions = 9
        self.MODEL_SAVE_DIR = 'saved-model'
        
        tf.reset_default_graph()
        self.session = tf.InteractiveSession()
        
        # Описание нейросети
        self.brain = MLP([self.observation_size,], [40, 30, self.num_actions], # [30, 20, num_actions]
                         [tf.nn.leaky_relu, tf.nn.leaky_relu, tf.identity])
        
        # Оптимизатор. RMSProp рекомендуется
        self.optimizer = tf.train.RMSPropOptimizer(learning_rate=0.01, decay=1)
        
        # Контроллер обучения с подкреплением
        self.current_controller = DiscreteDeepQ((self.observation_size,), self.num_actions, self.brain, self.optimizer, self.session,
                                                discount_rate=0.95, exploration_period=0, max_experience=100000,
                                                store_every_nth=1, train_every_nth=1, minibatch_size=32)
        
        
        # Восстановление состояния после перезагрузки
        if os.path.exists(self.MODEL_SAVE_DIR):
            print('restoring model...')
            self.current_controller.restore(self.MODEL_SAVE_DIR)
            print('done')
        
        
        # Последнее действие нейросети
        self.last_action = 0
        # Последнее наблюдаемое состояние среды
        self.last_observation = None
    
    def update(self):
        # print('update', self.object.worldPosition())
        if self.isFirst:
            action = self.predict(self.object.getObservation())
            self.object.getReward()
            self.object.setBotAction(action)
            self.isFirst = False
            return
        
        carReward = self.object.getReward()
        
        if not self.object.checkTimeCounter():
            print("timeout")
            carReward = -1
            # m_learnScript->learn(observation, carReward, true);
            self.learnFunc(None, carReward)
            self.object.reset()

            self.isFirst = True
            print("Game:", self.gameCounter, "Score:", self.score)
            self.score = 0
            self.gameCounter += 1
        elif not self.object.checkBackTriggerCounter():
            print("back triggers error")
            carReward -= 0.1
            # m_learnScript->learn(observation, carReward, true);
            self.learnFunc(None, carReward)
            self.object.reset()

            self.isFirst = True
            print("Game:", self.gameCounter, "Score:", self.score)
            self.score = 0
            self.gameCounter += 1
        elif not self.object.checkHasContact():
            print("contact error")
            carReward -= 0.1
            # m_learnScript->learn(observation, carReward, true);
            self.learnFunc(None, carReward)
            self.object.reset()

            self.isFirst = True
            print("Game:", self.gameCounter, "Score:", self.score)
            self.score = 0
            self.gameCounter += 1
        else:
            self.score += carReward
            # int action = m_learnScript->learn(observation, carReward, false);
            action = self.learnFunc(self.object.getObservation(), carReward)
            self.object.setBotAction(action);
    
    def stop(self):
        # self.current_controller.save(self.MODEL_SAVE_DIR)
        print('Model saved')
    
    # Основная функция для обучения. Предсказывает действие и обучает
    def learnFunc(self, _observation, _reward):
        if _observation is not None:
            _observation = np.array(_observation)

        if self.last_observation is not None:
            self.current_controller.store(self.last_observation, self.last_action, _reward, _observation)

        if _observation is not None:
            action = self.current_controller.action(_observation)
        else:
            action = 1

        with tf.device("/cpu:0"):
            self.current_controller.training_step()

        self.last_observation = _observation
        self.last_action = action

        return int(action)


    # Предсказать действие без обучения
    def predict(self, _observation):
        _observation = np.array(_observation)
        
        self.last_observation = _observation
        self.last_action = self.current_controller.action(_observation)
        return int(self.last_action)

