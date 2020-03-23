require 'test_helper'

class PumpControllerTest < ActionDispatch::IntegrationTest
  test "should get pump_status" do
    get pump_pump_status_url
    assert_response :success
  end

  test "should get set_pump_status" do
    get pump_set_pump_status_url
    assert_response :success
  end

end
