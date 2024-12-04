import os
import requests
import argparse
from dotenv import load_dotenv
from time import time
from datetime import datetime

# Load environment variables from .env files
load_dotenv()

# Get webhook URL from the environemt
WEBHOOK_URL = os.getenv('DISCORD_WEBHOOK_URL')
if not WEBHOOK_URL:
  raise ValueError("Discord webhook URL is not set. Please check your .env file.")

# Parse args
parser = argparse.ArgumentParser(description="Send Discord build notifications")
parser.add_argument('--status', required=True, choices=['start', 'success', 'failure'], help="Build status")
parser.add_argument('--start-time', type=float, help="Build start time as a timestamp")
parser.add_argument('--warnings', type=int, default=0, help="Number of warnings")
parser.add_argument('--errors', type=int, default=0, help="Number of errors")
parser.add_argument('--message', default='', help="Optional message")
args = parser.parse_args()

duration_minutes = 0
if args.start_time:
    duration_seconds = time() - args.start_time
    duration_minutes = duration_seconds / 60

# Prepare date and time
current_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")  # Format: YYYY-MM-DD HH:MM:SS

# Prepare message based on status
if args.status == 'start':
    message_content = f"🔄 **Build Started** at {current_time}"
elif args.status == 'success':
    message_content = (
        f"🎉 **Build Successful!** at {current_time}\n"
        f"🕒 **Duration:** {duration_minutes:.2f} minutes\n"
        f"⚠️ **Warnings:** {args.warnings}\n"
        f"❌ **Errors:** {args.errors}"
    )
elif args.status == 'failure':
    message_content = (
        f"❌ **Build Failed!** at {current_time}\n"
        f"🕒 **Duration:** {duration_minutes:.2f} minutes\n"
        f"⚠️ **Warnings:** {args.warnings}\n"
        f"❌ **Errors:** {args.errors}"
    )
if args.message:
    message_content += f"\n📋 **Message:** {args.message}"

# Send to Discord
payload = {
  "content": message_content
}

try:
    response = requests.post(WEBHOOK_URL, json=payload)
    response.raise_for_status()
    print("Notification sent successfully!")
except requests.exceptions.RequestException as e:
    print(f"Failed to send notification: {e}")