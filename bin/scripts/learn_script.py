import os
import numpy as np
import tensorflow as tf

from scripts.discrete_deepq import DiscreteDeepQ
from scripts.model import MLP


observation_size = 29
num_actions = 9
MODEL_SAVE_DIR = 'saved-model'


tf.reset_default_graph()
session = tf.InteractiveSession()

# Brain maps from observation to Q values for different actions.
# Here it is a done using a multi layer perceptron with 2 hidden
# layers
brain = MLP([observation_size,], [200, 200, num_actions], 
            [tf.tanh, tf.tanh, tf.identity])

# The optimizer to use. Here we use RMSProp as recommended
# by the publication
optimizer = tf.train.RMSPropOptimizer(learning_rate= 0.001, decay=0.9)

# DiscreteDeepQ object
current_controller = DiscreteDeepQ((observation_size,), num_actions, brain, optimizer, session,
                                   discount_rate=0.99, exploration_period=5000, max_experience=10000, 
                                   store_every_nth=4, train_every_nth=4)


if os.path.exists(MODEL_SAVE_DIR):
    print('restoring model...')
    current_controller.restore(MODEL_SAVE_DIR)
    print('done')


last_action = 0
last_observation = None


def learnFunc(_observation, _reward):
    global last_observation, last_action
    _observation = np.array(_observation)

    if last_observation is not None:
        current_controller.store(last_observation, last_action, _reward, _observation)

    action = current_controller.action(_observation)
    # global_simulation.perform_action(action)

    with tf.device("/cpu:0"):
        current_controller.training_step()

    last_observation = _observation
    last_action = action

    return int(action)


def predict(_observation):
    global last_observation, last_action
    _observation = np.array(_observation)
    
    last_observation = _observation
    last_action = current_controller.action(_observation)
    return int(last_action)


def saveModel():
    current_controller.save(MODEL_SAVE_DIR)
    print('Model saved')
