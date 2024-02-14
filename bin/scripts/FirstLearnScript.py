import os
os.environ['TF_ENABLE_ONEDNN_OPTS'] = '0'

import tensorflow as tf
import numpy as np
import keras
from keras.optimizers.legacy import Adam
from keras.models import load_model
import gym
import matplotlib.pyplot as plt

print(f'TensorFlow version: {tf.version.VERSION}')


class ReplayBuffer:
    def __init__(self, max_size, input_dims):
        self.mem_size = max_size
        self.mem_counter = 0

        self.state_memory = np.zeros((self.mem_size, *input_dims),
                                     dtype=np.float32)
        self.new_state_memory = np.zeros((self.mem_size, *input_dims),
                                         dtype=np.float32)
        self.action_memory = np.zeros((self.mem_size,), dtype=np.int32)
        self.reward_memory = np.zeros((self.mem_size,), dtype=np.float32)
        self.terminal_memory = np.zeros((self.mem_size,), dtype=np.int32)

    def store_transition(self, state, action, reward, state_, done):
        index = self.mem_counter % self.mem_size
        self.state_memory[index] = state
        self.new_state_memory[index] = state_
        self.reward_memory[index] = reward
        self.action_memory[index] = action
        self.terminal_memory[index] = 1 - int(done)
        self.mem_counter += 1

    def sample_buffer(self, batch_size):
        max_mem = min(self.mem_counter, self.mem_size)
        batch = np.random.choice(max_mem, batch_size, replace=False)

        state = self.state_memory[batch]
        state_ = self.new_state_memory[batch]
        rewards = self.reward_memory[batch]
        actions = self.action_memory[batch]
        terminal = self.terminal_memory[batch]

        return state, actions, rewards, state_, terminal

def build_dqn(lr, n_actions, input_dims, fc1_dims, fc2_dims, fc3_dims):
    model = keras.Sequential([
        keras.layers.Dense(fc1_dims, activation='relu'),
        keras.layers.Dense(fc2_dims, activation='relu'),
        keras.layers.Dense(fc3_dims, activation='relu'),
        keras.layers.Dense(n_actions, activation=None)
    ])
    model.compile(optimizer=Adam(learning_rate=lr), loss='mean_squared_error')

    return model


class Agent:
    def __init__(self, lr, gamma, n_actions, epsilon, batch_size,
                 input_dims, epsilon_dec=1e-4, epsilon_end=0.01,
                 mem_size=1_000_000, fname='my_checkpoint.tf', dirName='checkpoints'):
        self.action_space = list(range(n_actions))
        self.gamma = gamma
        self.epsilon = epsilon
        self.eps_min = epsilon_end
        self.eps_dec = epsilon_dec
        self.batch_size = batch_size
        self.model_file = dirName + '/' + fname
        self.model_dir = dirName
        self.memory = ReplayBuffer(mem_size, input_dims)

        self.q_eval = build_dqn(lr, n_actions, input_dims, 40, 30, 20)
        self.first_learn = True

    """
        if os.path.exists(self.model_dir):
            print('load model...')
            self.load_model()
            self.epsilon = self.eps_min
            print('model loaded')
    """

    def store_transition(self, state, action, reward, new_state, done):
        self.memory.store_transition(state, action, reward, new_state, done)

    def choose_action(self, observation):
        if np.random.random() < self.epsilon:
            action = np.random.choice(self.action_space)
        else:
            state = np.array([observation])
            actions = self.q_eval.predict(state)

            action = np.argmax(actions)

        return action

    def learn(self):
        if self.memory.mem_counter < self.batch_size:
            return

        states, actions, rewards, states_, dones = self.memory.sample_buffer(self.batch_size)

        q_eval = self.q_eval.predict(states)
        q_next = self.q_eval.predict(states_)

        q_target = np.copy(q_eval)
        batch_index = np.arange(self.batch_size, dtype=np.int32)

        q_target[batch_index, actions] = rewards + self.gamma * np.max(q_next, axis=1)*dones

        self.q_eval.train_on_batch(states, q_target)

        self.epsilon = self.epsilon - self.eps_dec if self.epsilon > self.eps_min else self.eps_min
        
        
        if self.first_learn is True:
            self.first_learn = False
            
            if os.path.exists(self.model_dir):
                print('load model...')
                self.load_model()
                self.epsilon = self.eps_min
                print('model loaded')

    def save_model(self):
        self.q_eval.save_weights(self.model_file)

    def load_model(self):
        self.q_eval.load_weights(self.model_file)


"""
new_file
"""

"""tf.compat.v1.disable_eager_execution()"""
tf.compat.v1.disable_eager_execution()
# env = gym.make('LunarLander-v2')
lr = 0.001
n_games = 500
agent = Agent(gamma=0.99, epsilon=1.0, lr=lr, input_dims=[29],
              n_actions=9, mem_size=1_000_000, batch_size=256,
              epsilon_end=0.01)

scores = []
eps_history = []
score = 0
last_action = 0
observation = []


def learnFunc(observation_, reward, done, info = dict()):
    global score, observation, last_action

    score += reward
    agent.store_transition(observation, last_action, reward, observation_, done)
    observation = observation_
    agent.learn()

    last_action = agent.choose_action(observation)
    return int(last_action)


def predict(_observation):
    global observation, last_action
    observation = _observation
    last_action = agent.choose_action(_observation)
    return int(last_action)


def saveModel():
    agent.save_model()
    print('Model saved')