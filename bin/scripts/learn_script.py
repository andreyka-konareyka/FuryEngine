import os
import numpy as np
import tensorflow as tf

from scripts.discrete_deepq import DiscreteDeepQ
from scripts.model import MLP


observation_size = 49
num_actions = 9
MODEL_SAVE_DIR = 'saved-model'


tf.reset_default_graph()
session = tf.InteractiveSession()

# Описание нейросети
brain = MLP([observation_size,], [40, 30, num_actions], # [30, 20, num_actions]
            [tf.nn.leaky_relu, tf.nn.leaky_relu, tf.identity])

# Оптимизатор. RMSProp рекомендуется
optimizer = tf.train.RMSPropOptimizer(learning_rate=0.01, decay=1)

# Контроллер обучения с подкреплением
current_controller = DiscreteDeepQ((observation_size,), num_actions, brain, optimizer, session,
                                   discount_rate=0.95, exploration_period=0, max_experience=100000, 
                                   store_every_nth=1, train_every_nth=1, minibatch_size=32)


# Восстановление состояния после перезагрузки
if os.path.exists(MODEL_SAVE_DIR):
    print('restoring model...')
    current_controller.restore(MODEL_SAVE_DIR)
    print('done')


# Последнее действие нейросети
last_action = 0
# Последнее наблюдаемое состояние среды
last_observation = None


# Основная функция для обучения. Предсказывает действие и обучает
def learnFunc(_observation, _reward):
    global last_observation, last_action
    if _observation is not None:
        _observation = np.array(_observation)

    if last_observation is not None:
        current_controller.store(last_observation, last_action, _reward, _observation)

    if _observation is not None:
        action = current_controller.action(_observation)
    else:
        action = 1

    with tf.device("/cpu:0"):
        current_controller.training_step()

    last_observation = _observation
    last_action = action

    return int(action)


# Предсказать действие без обучения
def predict(_observation):
    global last_observation, last_action
    _observation = np.array(_observation)
    
    last_observation = _observation
    last_action = current_controller.action(_observation)
    return int(last_action)


# Сохранение модели
def saveModel():
    current_controller.save(MODEL_SAVE_DIR)
    print('Model saved')