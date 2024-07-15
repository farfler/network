import farfler_network as fn
import time

# Create a network node
io_context = fn.IOContext()
network = fn.Network(io_context, "node")

def subscriber_callback(message):
  print(f"Published temperature: {message}")

# Create a subscriber
network.subscribe_float("temperature", subscriber_callback)
print("Subscribed to temperature messages")

while True:
  # Publish a message
  network.publish_float("temperature", 22.5)
  time.sleep(1)
