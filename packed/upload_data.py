import os
from dotenv import load_dotenv
from firebase import Firebase

print('Starting application...')
print('Loading environment...', end='\r')
load_dotenv()
print('✓ Loaded environment')

# Firebase credentials
FIREBASE_KEY_PATH = os.getenv("FIREBASE_KEY_PATH")
FIREBASE_DATABASE_URL = os.getenv("FIREBASE_DATABASE_URL")

# cred = credentials.Certificate(FIREBASE_KEY_PATH)
# firebase_admin.initialize_app(cred, {
#     'databaseURL': FIREBASE_DATABASE_URL
# })

# Initialize Firebase app
print('Initializing firebase...', end='\r')
firebase = Firebase(
  service_file_path=FIREBASE_KEY_PATH, 
  database_url=FIREBASE_DATABASE_URL
)
print('✓ Initialized firebase')


# print('Sending data...', end='\r')
# data = {
#   '12-01-2024': {
#     'weight': 12, 
#     'name': 'John',
#   }
# }

# firebase.set_data(data)
# print('✓ Sent data')

print('Retrieving data...', end='\r')
print(firebase.fetch_data())
print('✓ Retrieved data')

# ref = db.reference('test/data')
# ref.set(data)
