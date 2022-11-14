import openai

key_file = open("api.key", "r")
api_key = key_file.read().strip()

openai.api_key = api_key

response = openai.Image.create(
    prompt="Sylvain mange un caillou",
    n=1,
    size="1024x1024")

print(response['data'][0]['url'])