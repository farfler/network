import farfler_network as fn

# Create a network node
io_context = fn.IOContext()
network = fn.Network(io_context, "node")
print("Network node created")

def subscriber_callback(message):
  print(f"Received temperature: {message}")

# Create a subscriber
network.subscribe_float("temperature", subscriber_callback)
print("Subscribed to temperature messages")

while True:
  pass