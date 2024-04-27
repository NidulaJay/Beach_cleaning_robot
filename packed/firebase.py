import firebase_admin
from firebase_admin import credentials, db

class Firebase:
  parent_ref = None
  database_url = None
  service_file_path = None
  temperature_ref = '/temperature'
  angle_ref = '/angle'
  weight_ref = '/weight'


  def __init__(self, database_url ,service_file_path) -> None:
    self.database_url = database_url
    self.service_file_path = service_file_path
    cred = credentials.Certificate(service_file_path)
    firebase_admin.initialize_app(cred, {
        'databaseURL': database_url
    })
    self.parent_ref = 'robot'

  def get_all(self):
    data_ref = db.reference(self.parent_ref)
    data = data_ref.get()
    return data

  def set_weight(self, value):
    weight_ref = db.reference(self.parent_ref+self.weight_ref)
    weight_ref.set(value)
  
  def get_weight(self):
    weight_ref = db.reference(self.parent_ref+self.weight_ref)
    data = weight_ref.get()
    return data
  
  def set_temperature(self, value):
    temperature_ref = db.reference(self.parent_ref+self.temperature_ref)
    temperature_ref.set(value)
  
  def get_temperature(self):
    temperature_ref = db.reference(self.parent_ref+self.temperature_ref)
    data = temperature_ref.get()
    return data
  
  def set_angle(self, value):
    angle_ref = db.reference(self.parent_ref+self.angle_ref)
    angle_ref.set(value)
  
  def get_angle(self):
    angle_ref = db.reference(self.parent_ref+self.angle_ref)
    data = angle_ref.get()
    return data