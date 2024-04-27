import os
from time import sleep
import streamlit as st
import json
import plotly.express as px
import pandas as pd
import matplotlib.pyplot as plt
from dotenv import load_dotenv
from streamlit_plotly_events import plotly_events

from firebase import Firebase

# Load environment variables from .env file
load_dotenv()

# Firebase credentials
FIREBASE_KEY_PATH = os.getenv("FIREBASE_KEY_PATH")
FIREBASE_DATABASE_URL = os.getenv("FIREBASE_DATABASE_URL")
prev_data = {
    'temperature': 0,
    'weight': 0,
    'angle': 0,
    'humidity': 0

}

st.title("Beach garbage collector daily stats")
st.write("Fetching data from Firebase...")
col1, col2, col3 = st.columns(3)


def refresh_data():
    global prev_data
    print('Retrieving data...', end='\r')
    data: dict = firebase.get_all()
    print(data)
    print('✓ Retrieved data')

    try:
        col1.metric("Temperature", f"{data['temperature']} °C", f"{data['temperature'] - prev_data['temperature']} °C")
        col2.metric("Weight", f"{data['weight']} KG", f"{data['weight'] - prev_data['weight']} KG")
        col3.metric("Humidity", f"{data['humidity']}%", f"{data['humidity'] - prev_data['humidity']}%")
    except KeyError as e:
        print(e)
    prev_data = data.copy()

# Streamlit web app
def main():
    while True:
        refresh_data()
        sleep(3000)

if __name__ == "__main__":
    print("Starting application...")
    print('Initializing firebase...', end='\r')
    firebase = Firebase(
      service_file_path=FIREBASE_KEY_PATH, 
      database_url=FIREBASE_DATABASE_URL
    )
    print('✓ Initialized firebase')
    main()
